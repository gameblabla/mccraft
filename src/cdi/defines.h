#ifndef DEFINES_H
#define DEFINES_H

#define FLOATING_POINT 1
#define RGB565(r, g, b) (((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3))

#ifndef FLOATING_POINT
#define accum _Accum
//This controls the quality of the rays. The higher, the better the quality
//Decreasing this too much will also increase the amount of glitches in the render
#define STEP_RESOLUTION 16.0k
#else
//This controls the quality of the rays. The higher, the better the quality
//Decreasing this too much will also increase the amount of glitches in the render
#define STEP_RESOLUTION 16.0f
#endif

#define SCREEN_GAME_WIDTH 384
#define SCREEN_GAME_HEIGHT 240

/*
 * Multiplications on some CPU architectures (V810 for instance)
 * are much slower and can take up to 13 cycles versus 3/4.
*/
//#define PROCESS_BLOCK_MULT (block * 256)
#define PROCESS_BLOCK_MULT (block << 8) + 1

#ifdef _16BIT
#error "CDI only supports 8bpp/CLUT8"
#endif

#define SKY_RGB_COLOR 0x67
#define VOID_RGB_COLOR 0x45
#define EMPTY_RGB_COLOR 0x29
#define TEXTURE_ARRAY mccraft_texture_8bit
#define bufSetPixel(buffer,x,y,color) *((uint8_t*)buffer + x + y * SCREEN_GAME_WIDTH) = color;
#define BPP_BITDEPTH 8
static uint8_t color_texture;

//Don't make any of these larger then 120 and make sure that the world can even still fit into RAM
//world size (in bytes) = WORLD_WIDTH * WORLD_HEIGHT * WORLD_DEPTH
#define WORLD_WIDTH 16
#define WORLD_HEIGHT 16
#define WORLD_DEPTH 16

#endif
