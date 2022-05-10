#include <stdio.h>
#include <memory.h>
#include "sys.h"
#include "video.h"
#include "graphics.h"

ImagePlane ipB1, ipB2;
ImagePlane *ipActive = &ipB1, *ipDraw = &ipB2;

u_char *spriteSheet;
int curICF = ICF_MAX;

u_char *hudBuffer;

void fillBuffer(buffer, data, size)
	u_int *buffer, data, size;
{
	u_int i;
	for (i = 0; i < (size / 4); i++) {
		buffer[i] = data;
	}
}

void fillVideoBuffer(videoBuffer, data)
	u_int *videoBuffer, data;
{
	fillBuffer(videoBuffer, data, VBUFFER_SIZE);
}

void createVideoBuffers()
{
	ipB1.videoBuffer = (u_char*)srqcmem(VBUFFER_SIZE, VIDEO2);
	ipB2.videoBuffer = (u_char*)srqcmem(VBUFFER_SIZE, VIDEO2);
	
	fillVideoBuffer(ipB1.videoBuffer, 0);
	fillVideoBuffer(ipB2.videoBuffer, 0);

	//dc_wrli(videoPath, lctA, 1, 0, cp_dadr((int)spA1.videoBuffer + pixelStart));
	dc_wrli(videoPath, lctA, 1, 1, cp_sig());
	dc_wrli(videoPath, lctB, 1, 0, cp_dadr((int)ipB1.videoBuffer + pixelStart));
	dc_exec(videoPath, fctA, fctB);
}

void loadPalette(plane, filename)
    int plane;
    char *filename;
{
    int file = open(filename, READ_);
	read(file, fctBuffer, sizeof(int) * 130);
	close(file);
	dc_wrfct(videoPath, (plane == PA) ? fctA : fctB, 0, 130, fctBuffer);
	dc_exec(videoPath, fctA, fctB);
}

void loadImage(file, videoBuffer)
    int file;
	u_char *videoBuffer;
{
	read(file, videoBuffer, VBUFFER_SIZE);
}

void drawTile(tile, position, videoBuffer) 
	u_char tile;
	u_int position;
	u_char *videoBuffer;
{
	u_char* src = spriteSheet + ((tile & 0x0f) << 4) + ((tile & 0xf0) << 8);
	u_char* dst = videoBuffer + position;

	memcpy(dst +  0 * SCREEN_WIDTH, src +  0 * 256, 16);
	memcpy(dst +  1 * SCREEN_WIDTH, src +  1 * 256, 16);
	memcpy(dst +  2 * SCREEN_WIDTH, src +  2 * 256, 16);
	memcpy(dst +  3 * SCREEN_WIDTH, src +  3 * 256, 16);
	memcpy(dst +  4 * SCREEN_WIDTH, src +  4 * 256, 16);
	memcpy(dst +  5 * SCREEN_WIDTH, src +  5 * 256, 16);
	memcpy(dst +  6 * SCREEN_WIDTH, src +  6 * 256, 16);
	memcpy(dst +  7 * SCREEN_WIDTH, src +  7 * 256, 16);
	memcpy(dst +  8 * SCREEN_WIDTH, src +  8 * 256, 16);
	memcpy(dst +  9 * SCREEN_WIDTH, src +  9 * 256, 16);
	memcpy(dst + 10 * SCREEN_WIDTH, src + 10 * 256, 16);
	memcpy(dst + 11 * SCREEN_WIDTH, src + 11 * 256, 16);
	memcpy(dst + 12 * SCREEN_WIDTH, src + 12 * 256, 16);
	memcpy(dst + 13 * SCREEN_WIDTH, src + 13 * 256, 16);
	memcpy(dst + 14 * SCREEN_WIDTH, src + 14 * 256, 16);
	memcpy(dst + 15 * SCREEN_WIDTH, src + 15 * 256, 16);
}

void loadTilemap(videoBuffer, tilemap)
	u_char *videoBuffer;
	u_char *tilemap;
{
	u_short x;
	u_short y;
	u_short i = 0;
	u_int origin;

	for (y = 0; y < TILES_Y; y++) {
		for (x = 0; x < TILES_X; x++) {
			drawTile(
				tilemap[i++],
				pixel_pos(x << 4, y << 4),
				videoBuffer
			);
		}
	}
}


void swapImageBuffer()
{
	ImagePlane* temp = ipDraw;
	ipDraw = ipActive;
	ipActive = temp;
	dc_wrli(videoPath, lctB, 1, 0, cp_dadr((int)ipActive->videoBuffer + pixelStart));
}


void setICF(value)
	int value;
{
	if      (value > ICF_MAX) curICF = ICF_MAX;
	else if (value < ICF_MIN) curICF = ICF_MIN;
	else                      curICF = value;

	dc_wrli(videoPath, lctA, 1, 7, cp_icf(PA, curICF));
	dc_wrli(videoPath, lctB, 1, 7, cp_icf(PB, curICF));
}

void initGraphics()
{
	createVideoBuffers();
	loadPalette(PA, "PLANE_A.PAL");
	loadPalette(PB, "PLANE_B.PAL");
}
