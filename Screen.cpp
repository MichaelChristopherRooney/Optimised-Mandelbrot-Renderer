/*
 * 
 * DO NOT MODIFY ANYTHING IN THIS FILE!
 * 
 *
 */

#include <iostream>

#include <SDL/SDL.h>

#include "Screen.h"

bool Screen::init()
{

	int bpp;
	Uint32 video_flags;

	atexit(SDL_Quit);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		std::cerr << "Screen::init : Couldn't initialize SDL: "<< SDL_GetError() << std::endl;
		return false;
	}

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY ,SDL_DEFAULT_REPEAT_INTERVAL);

	/* See if we should detect the display depth */
	if (SDL_GetVideoInfo()->vfmt->BitsPerPixel <= 8) {
		bpp = 8;
	} else {
		bpp = 32;
	}

	video_flags = SDL_HWSURFACE;
	
	if ((surface = SDL_SetVideoMode(width, height, bpp, video_flags)) == NULL) {
		std::cerr << "Screen::init : Couldn't initialize Video: "<< SDL_GetError() << std::endl;
		return false;
	}

	std::cout << "Screen::init complete"<< std::endl;

	return true;
}

void Screen::putpixel(int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	Uint32 pixel = (r << 16) + (g << 8)+ b;

	Uint32 *ptr = (Uint32*)surface->pixels;
	int lineoffset = y * (surface->pitch / 4);
	ptr[lineoffset + x] = pixel;

}

void Screen::flip()
{
	SDL_Flip(surface);
}
