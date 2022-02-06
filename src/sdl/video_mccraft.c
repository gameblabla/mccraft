#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "defines.h"

#define FPS_VIDEO 30
const float real_FPS = 1000/FPS_VIDEO;
static Uint32 start;
SDL_Surface* screen;

int Init_video()
{
	SDL_Surface* tmp;
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, BPP_BITDEPTH, SDL_SWSURFACE);
	
	#ifndef _16BIT
	tmp = SDL_LoadBMP("data/final.bmp");
	SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, tmp->format->palette->colors, 0, 256);
	SDL_FreeSurface(tmp);
	#endif
}

void Start_video()
{
	start = SDL_GetTicks();
}

void Refresh_video()
{
	SDL_Flip(screen);
		
	if (real_FPS > SDL_GetTicks()-start) SDL_Delay(real_FPS-(SDL_GetTicks()-start));
}

void Quit_video()
{
	SDL_FreeSurface(screen);
	SDL_Quit();
}
