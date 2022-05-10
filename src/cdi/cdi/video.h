#ifndef __VIDEO_H__
#define	__VIDEO_H__


#include "types.h"

#define FCT_SIZE 132
#define LCT_SIZE 280 * 2 + 2

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 280
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#define VBUFFER_SIZE 108544 /* First full block after SCREEN_WIDTH * SCREEN_HEIGHT */

extern int videoPath;
extern int fctA, fctB, lctA, lctB;

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
extern u_int fctBuffer[FCT_SIZE];
extern u_int pixelStart;

#endif
