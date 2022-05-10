#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "graphics.h"

#define FRAMEBUFFER_GAME (uint8_t*)ipDraw->videoBuffer

extern int Init_video();
extern void Start_video();
extern void Refresh_video();
extern void Quit_video();

#endif
