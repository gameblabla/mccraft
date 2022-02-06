#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define FRAMEBUFFER_GAME framebuffer_game

extern uint8_t* framebuffer_game;

extern int Init_video();
extern void Start_video();
extern void Refresh_video();
extern void Quit_video();

#endif
