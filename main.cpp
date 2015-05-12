#include <xmmintrin.h>
#include <iostream>
#include <cmath>
#include <sys/time.h>
#include "Screen.h"

const int MAX_ITS = 1000; //Max Iterations before we assume the point will not escape
const int HXRES = 700; // horizontal resolution	
const int HYRES = 700; // vertical resolution
const int MAX_DEPTH = 40; // max depth of zoom
const float ZOOM_FACTOR = 1.02; // zoom between each frame
const float PX = -0.702295281061; // Centre point we'll zoom on - Real component
const float PY = +0.350220783400; // Imaginary component

/*
The palette used to colour the render
data represents RGB triplets
 */

const int PAL_SIZE = 40;  //Number of entries in the palette 

unsigned char pal[]={
	255,180,4,
	240,156,4,
	220,124,4,
	156,71,4,
	72,20,4,
	251,180,4,
	180,74,4,
	180,70,4,
	164,91,4,
	100,28,4,
	191,82,4,
	47,5,4,
	138,39,4,
	81,27,4,
	192,89,4,
	61,27,4,
	216,148,4,
	71,14,4,
	142,48,4,
	196,102,4,
	58,9,4,
	132,45,4,
	95,15,4,
	92,21,4,
	166,59,4,
	244,178,4,
	194,121,4,
	120,41,4,
	53,14,4,
	80,15,4,
	23,3,4,
	249,204,4,
	97,25,4,
	124,30,4,
	151,57,4,
	104,36,4,
	239,171,4,
	131,57,4,
	111,23,4,
	4,2,4
};

/*
check if the given points are members of the set
*/
__m128 member(__m128 cx, __m128 cy){

	__m128 x = _mm_set1_ps(0.0f);
	__m128 y = _mm_set1_ps(0.0f);
	__m128 xTemp;

	/* used in evaluating the while loop's termination condition */
	__m128 four = _mm_set1_ps(4.0f);

	/* used in calculating y */
	__m128 two = _mm_set1_ps(2.0f);

	__m128 iterationsVec = _mm_set1_ps(0.0f);
	__m128 iterationsValues;
	__m128 mask = _mm_set1_ps(1.0f);
	
	int iterations = 0;

	/*
	get the result of x*x + y*y < 4 into a mask and evaluate it
	also check that the max number of iterations has not been exceeded
	*/
	while((_mm_movemask_ps(iterationsValues = (_mm_cmplt_ps(_mm_add_ps( _mm_mul_ps(x,x), _mm_mul_ps(y,y)), four)))) != 0 && iterations < MAX_ITS){
      
		// iterationsVec += (iterationValues AND 1111)
		iterationsVec = _mm_add_ps(iterationsVec, _mm_and_ps(iterationsValues, mask));

		xTemp = _mm_add_ps( _mm_sub_ps( _mm_mul_ps(x,x), _mm_mul_ps(y,y)), cx); // x*x - y*y + cx
		y = _mm_add_ps( _mm_mul_ps( _mm_mul_ps(x,y), two), cy); // y = (2 * x * y) + cy

		x = xTemp;
		iterations++;

	}

   return iterationsVec;

}

int main(){

	float mag=1.0; // initial  magnification

	// Create a screen to render to
	Screen *screen;
	screen = new Screen(HXRES, HYRES);

	int depth=0;

	struct timeval startTime;
	struct timeval stopTime;
	long long totalTime = 0;

	// these values never have to be recalculated so we put them outside of the while loop 

	// the reciprocal is found so multiplication can be used instead of a slower divide later on
	__m128 hxresVec = _mm_set1_ps((float) 1.0f / HXRES);
	__m128 hyresVec = _mm_set1_ps((float) 1.0f / HYRES);

	__m128 pointMinusFive = _mm_set1_ps(-0.5f);
	__m128 four = _mm_set1_ps(4.0f);	

	while (depth < MAX_DEPTH) {

	    // record starting time
	    gettimeofday(&startTime, NULL);

		// these values only change when m does, so put them outside of the for loops

		__m128 fourOverM = _mm_set1_ps(mag);
		fourOverM = _mm_div_ps(four, fourOverM); // (4/mag)

		__m128 PXoverFourOverM = _mm_set1_ps(PX);
		PXoverFourOverM = _mm_div_ps(PXoverFourOverM, fourOverM); // (px/(4/mag))

		__m128 PYoverFourOverM = _mm_set1_ps(PY);
		PYoverFourOverM = _mm_div_ps(PYoverFourOverM, fourOverM); // (py/(4/mag))

		#pragma omp parallel for schedule(dynamic, 1)		
		for (int hy=0; hy<HYRES; hy++) {

			// these values only change when hy does, so place them outside the inner for loop

			__m128 cy = _mm_set1_ps(hy);

			cy = _mm_mul_ps(cy, hyresVec);
			cy = _mm_add_ps(cy, pointMinusFive); // (hy/HYRES) + (-0.5)
			cy = _mm_add_ps(cy, PYoverFourOverM); // (hy/HYRES) + (-0.5) + (PY/(4.0/mag))
			cy = _mm_mul_ps(cy, fourOverM); // ( (hy/HYRES) + (-0.5) + (PY/(4.0/mag)) ) * (4.0/mag)


			for (int hx=0; hx<HXRES; hx += 4) {

				__m128 cx = _mm_setr_ps(hx, hx+1, hx+2, hx+3);

				cx = _mm_mul_ps(cx, hxresVec); // (hx/HXRES)
				cx = _mm_add_ps(cx, pointMinusFive); // (hx/HXRES) + (-0.5)
				cx = _mm_add_ps(cx, PXoverFourOverM); // (hx/HXRES) + (-0.5) + (PX/(4.0/mag))
				cx = _mm_mul_ps(cx, fourOverM); // ( (hx/HXRES) + (-0.5) + (PX/(4.0/mag)) ) * (4.0/mag)
				
				// get the __m128 returned from member() and turn it into a float[]
				float storeFloat[4];
				_mm_store_ps(storeFloat, member(cx, cy));

				/// take the 4 values from the float[] and colour a pixel based on them
				for(int i = 0; i < 4; i++){

					if (storeFloat[i] != MAX_ITS) {
						int index = (((int)storeFloat[i]%40) - 1) * 3;
						screen->putpixel(hx + i, hy, pal[index], pal[index+1], pal[index+2]);
					} else {
						screen->putpixel(hx + i, hy, 0, 0, 0);
					}

				}

			}

		}

		// record end time and add to total time
		gettimeofday(&stopTime, NULL);
		totalTime += (stopTime.tv_sec - startTime.tv_sec) * 1000000L + (stopTime.tv_usec - startTime.tv_usec);

		// Show the rendered image on the screen 
		screen->flip();
		std::cout << "Render done. Depth: " << depth << ". Magnification: " << mag << std::endl;

		// zoom and move to next depth
		mag *= ZOOM_FACTOR;
		depth++;

	}
	
	std::cout << "Total executing time " << totalTime << " microseconds\n";

}
