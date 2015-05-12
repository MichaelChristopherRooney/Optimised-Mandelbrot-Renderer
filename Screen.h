/*
 * 
 * DO NOT MODIFY ANYTHING IN THIS FILE!
 * 
 *
 */


#ifndef SCREEN_H_
#define SCREEN_H_

#include <SDL/SDL.h>

class Screen
{
	public:
		Screen(int Width, int Height) :
			width(Width), height(Height)
		{
			pixels = new Uint32[width * height];

			init();
		}

		~Screen()
		{
			delete [] pixels;
		};

		bool init();
		void putpixel(int x, int y, Uint8 r, Uint8 g, Uint8 b);

		void flip();

	private:

		int width;
		int height;
		SDL_Surface *surface, *surface2;
		Uint32 *pixels;

};

#endif /*SCREEN_H_*/
