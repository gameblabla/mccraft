#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>

extern SDL_Surface* screen;

extern int Init_video();
extern void Start_video();
extern void Refresh_video();
extern void Quit_video();

#endif
