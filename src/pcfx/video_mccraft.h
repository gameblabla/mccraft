#ifndef GAME_H
#define GAME_H

#include "defines.h"

#define FRAMEBUFFER_GAME framebuffer
extern unsigned char framebuffer[(SCREEN_GAME_WIDTH*SCREEN_GAME_HEIGHT)];

extern int Init_video();
extern void Start_video();
extern void Refresh_video();
extern void Quit_video();

#endif
