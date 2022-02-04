/*
 * MCTest.c
 *
 * Created: 2/23/2019 4:41:28 PM
 *  Author: lucah
 */
#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "textures.h"

#define FLOATING_POINT 1

#ifndef FLOATING_POINT
#define accum _Accum
#endif

Uint32 start;

#define FPS_VIDEO 60
const float real_FPS = 1000/FPS_VIDEO;

//Don't make any of these larger then 120 and make sure that the world can even still fit into RAM
//world size (in bytes) = WORLD_WIDTH * WORLD_HEIGHT * WORLD_DEPTH
#define WORLD_WIDTH 16
#define WORLD_HEIGHT 16
#define WORLD_DEPTH 16

#define bufSetPixel(buffer,x,y,color) *((uint16_t*)buffer + x + y * width) = color;

#ifdef FLOATING_POINT
float toRadians(float deg){
	return deg * 0.017453292519943295f;
}
#else
accum toRadians(accum deg){
	return deg * 0.017453292519943295K;
}
#endif

//The World
uint8_t blocks[WORLD_WIDTH][WORLD_HEIGHT][WORLD_DEPTH];

SDL_Surface* screen;

void GenerateWorld()
{
	//Put some stuff in the world to be rendered
	for(uint8_t i = 0; i < WORLD_WIDTH; i++) {
		for(uint8_t j = 0; j < WORLD_DEPTH; j++) {
			for(uint8_t k = 0; k < 5; k++) blocks[i][k][j] = 1;
			blocks[i][5][j] = 2;
			blocks[i][6][j] = 3;
		}
	}
	//A "Hill"
	blocks[5][6][5] = 2;
	blocks[6][6][5] = 2;
	blocks[5][6][6] = 2;
	blocks[5][7][5] = 3;
	blocks[6][7][5] = 3;
	blocks[5][7][6] = 3;
	
	//Some random rocks
	blocks[12][6][3] = 2;
	blocks[12][7][3] = 1;
	blocks[12][6][2] = 2;
	blocks[12][7][2] = 1;
	
	//A hole
	blocks[7][6][7] = 0;
	blocks[6][6][7] = 0;
	blocks[7][6][6] = 0;
	blocks[6][6][6] = 0;
	//House
	for(uint8_t i = 1; i < 7; i++) {
		blocks[i + 8][7][1 + 4] = 4;
		blocks[i + 8][7][6 + 4] = 4;
		blocks[i + 8][8][1 + 4] = 4;
		blocks[i + 8][8][6 + 4] = 4;
		blocks[i + 8][9][1 + 4] = 4;
		blocks[i + 8][9][6 + 4] = 4;
	}
	for(uint8_t i = 1; i < 7; i++) {
		blocks[6 + 8][7][i + 4] = 4;
		blocks[6 + 8][8][i + 4] = 4;
		if(i != 3) {
			blocks[1 + 8][7][i + 4] = 4;
			if(i != 5) blocks[1 + 8][8][i + 4] = 4;
		}
		blocks[1 + 8][9][i + 4] = 4;
		blocks[6 + 8][9][i + 4] = 4;
		blocks[1 + 8][6][i + 4] = 2;
		blocks[6 + 8][6][i + 4] = 2;
	}
	for(uint8_t i = 2; i < 6; i++) {
		blocks[i + 8][10][1 + 5] = 4;
		blocks[i + 8][10][6 + 3] = 4;
	}
	for(uint8_t i = 2; i < 6; i++) {
		blocks[1 + 9][10][i + 4] = 4;
		blocks[6 + 7][10][i + 4] = 4;
	}
	for(uint8_t i = 3; i < 5; i++) {
		blocks[i + 8][11][1 + 6] = 4;
		blocks[i + 8][11][6 + 2] = 4;
	}
	for(int i = 3; i < 5; i++) {
		blocks[1 + 10][11][i + 4] = 4;
		blocks[6 + 6][11][i + 4] = 4;
	}
	for(uint8_t i = 1; i < 7; i++) {
		for(uint8_t j = 1; j < 7; j++) {
			blocks[i + 8][6][j + 4] = 1;
		}
	}
	for(uint8_t i = 0; i < 3; i++) {
		blocks[9][7 + i][1 + 4] = 5;
		blocks[14][7 + i][1 + 4] = 5;
		blocks[9][7 + i][6 + 4] = 5;
		blocks[14][7 + i][6 + 4] = 5;
	}
	//Tree
	for(uint8_t i = 7; i < 12; i++) {
		for(uint8_t j = 11; j < 16; j++) {
			blocks[i][10][j] = 6;
			blocks[i][11][j] = 6;
		}
	}
	for(uint8_t i = 8; i < 11; i++) {
		blocks[i][12][13] = 6;
		blocks[i][13][13] = 6;
	}
	for(uint8_t i = 12; i < 15; i++) {
		blocks[9][12][i] = 6;
		blocks[9][13][i] = 6;
	}
	for(uint8_t i = 7; i < 13; i++) {
		blocks[9][i][13] = 5;
	}
	blocks[9][6][13] = 2;
}

int main(void){
	FILE* fp;
	int width = 256;
	int height = 224;
	//This controls the quality of the rays. The higher, the better the quality
	//Decreasing this too much will also increase the amount of glitches in the render
	uint8_t stepResolution = 16;
	
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(width, height, 16, SDL_SWSURFACE);
	atexit(SDL_Quit);
	
	GenerateWorld();
	//printf("Initialized.\r\n");*
	
	//printf("Final image dimensions: %ux%u\r\n", width, height);d

	//Player position
	#ifdef FLOATING_POINT
	float x,y,z = 0;
	float rotX,rotY = 0.0f;
	y = 9;
	x = 0;
	rotY = 0.0f;
	rotX = 0.0f;
	#else
	accum x,y,z = 0;
	accum rotX,rotY = 0;
	y = 9;
	x = 0;
	rotY = 0;
	rotX = 0;
	#endif
				
	//Working variables
	#ifdef FLOATING_POINT
	float cosX,cosY,sinX,sinY;
	float toImageX,toImageY,toImageZ,toImageXn,toImageYn,toImageZn;
	float length;
	float stepX,stepY,stepZ;
	float s1;
	float pixelFX,pixelFY,pixelFZ;
	float wX,wY,wZ;
	#else
	accum cosX,cosY,sinX,sinY;
	accum toImageX,toImageY,toImageZ,toImageXn,toImageYn,toImageZn;
	accum length;
	accum stepX,stepY,stepZ;
	accum s1;
	accum pixelFX,pixelFY,pixelFZ;
	accum wX,wY,wZ;
	#endif
	signed char intX,intY,intZ;
	signed char prevIntX,prevIntY,prevIntZ;
	uint8_t bb;
	uint8_t block;
	uint8_t pixelX,pixelY,pixelZ;
	uint8_t bc,bd,be,bf;
	uint16_t stepCntr = 0;
	
	uint8_t skyR = 0xAB;
	uint8_t skyG = 0xCD;
	uint8_t skyB = 0xEF;
	
	uint8_t voidR = 0x8B;
	uint8_t voidG = 0xA6;
	uint8_t voidB = 0xC1;
	
	uint8_t currR,currG,currB;
	
	#ifdef FLOATING_POINT
	/*rotX = 4.2300005f;
	rotY = 99.70001f;
	x = 1.7884214f;
	z = 9.30016f;*/
	rotX = -0.0900002f;
	rotY = 47.365f;
	x = 0.404446f;
	z = 0.22908628f;
	#else
	//Coords for standing in front of the house
	//rotX = 4.2300005K;
	//rotY = 99.70001K/* + 9.0K * (accum)i2*/;
	//x = 1.7884214K;
	//z = 9.30016K;
	//Coords for looking at the house and tree at once
	rotX = -0.0900002K;
	rotY = 47.365K;
	x = 0.404446K;
	z = 0.22908628K;
	#endif
	
	cosX = cos(toRadians(rotX));
	cosY = cos(toRadians(rotY));
	sinX = sin(toRadians(rotX));
	sinY = sin(toRadians(rotY));
	
	
	while(1)
	{
		start = SDL_GetTicks();
		
		#ifdef FLOATING_POINT
		rotY+= 1.1f;
		#else
		rotY+= 1.1K;
		#endif
		cosX = cos(toRadians(rotX));
		cosY = cos(toRadians(rotY));
		sinX = sin(toRadians(rotX));
		sinY = sin(toRadians(rotY));
		
		for(int j = height - 1; j >= 0; j--)
		{
			for(int i = 0; i < width; i++)
			{
				//Raytrace!
				#ifdef FLOATING_POINT
				toImageX = ((float)i / (float)width - 0.5f);
				toImageY = -((float)j / (float)width - 0.5f);
				toImageZ = 1.0f;
				#else
				toImageX = ((accum)i / (accum)width - 0.5K);
				toImageY = -((accum)j / (accum)width - 0.5K);
				toImageZ = 1.0K;
				#endif
								
				toImageYn = toImageY * cosX - toImageZ * sinX;
				toImageZ = toImageY * sinX + toImageZ * cosX;
				toImageY = toImageYn;
								
				toImageXn = toImageX * cosY + toImageZ * sinY;
				toImageZ = -toImageX * sinY + toImageZ * cosY;
				toImageX = toImageXn;
				
				length = sqrt(toImageX * toImageX + toImageY * toImageY + toImageZ * toImageZ);
				toImageX /= length;
				toImageY /= length;
				toImageZ /= length;
				toImageXn = toImageX;
				toImageYn = toImageY;
				toImageZn = toImageZ;
				#ifdef FLOATING_POINT
				toImageX /= (float)stepResolution;
				toImageY /= (float)stepResolution;
				toImageZ /= (float)stepResolution;
				#else
				toImageX /= (accum)stepResolution;
				toImageY /= (accum)stepResolution;
				toImageZ /= (accum)stepResolution;
				#endif
				bb = 1;
				
				stepX = x;
				stepY = y;
				stepZ = z;
				intX = (signed char)x + 1;
				intY = (signed char)y;
				intZ = (signed char)z;
				currR = skyR;
				currG = skyG;
				currB = skyB;
				stepCntr = 0;
				
				while(1)
				{
					prevIntX = intX;
					prevIntY = intY;
					prevIntZ = intZ;
					intX = (signed char)stepX;
					intY = (signed char)stepY;
					intZ = (signed char)stepZ;
					if(prevIntX != intX || prevIntY != intY || prevIntZ != intZ) {
						if(intX >= WORLD_WIDTH || intX < 0 || intY >= WORLD_HEIGHT || intY < 0 || intZ >= WORLD_DEPTH || intZ < 0) {
							#ifdef FLOATING_POINT
							if(toImageY < -0.125f / (float)stepResolution) {
								currR = voidR;
								currG = voidG;
								currB = voidB;
							}
							#else
							if(toImageY < -0.125K / (accum)stepResolution) {
								currR = voidR;
								currG = voidG;
								currB = voidB;
							}
							#endif
							break;
						}
						if(bb || stepCntr >= stepResolution) {
							/*found = 0;
							bc = toImageX > 0 ? 1 : 0;
							be = toImageY > 0 ? 1 : 0;
							bf = toImageZ > 0 ? 1 : 0;
							startZ = bf ? 0 : -1;
							if(intZ == 0) startZ = 0;
							endZ = bf;
							if(intZ == WORLD_DEPTH - 1) endZ = 0;
							for(signed char a2 = bc ? 0 : -1; a2 <= bc; a2++) {
								if(intX + a2 < 0 || intX + a2 >= WORLD_WIDTH) continue;
								for(signed char b = be ? 0 : -1; b <= be; b++) {
									if(intY + b < 0 || intY + b >= WORLD_HEIGHT) continue;
									for(signed char c = startZ; c <= endZ; c++) {
										if(blocks[intX + a2][intY + b][intZ + c] != 0) {
											if(bb){
												bb = 0;
												b = 2;
												a2 = 2;
												stepX -= toImageXn;
												stepY -= toImageYn;
												stepZ -= toImageZn;
											}
											found = 1;
											break;
										}
									}
								}
							}
							if(found == 0) bb = 1;*/
							
							//This is basically the above code, but convoluted as fck.....but somehow it's faster
							bc = toImageY > 0 && intY < WORLD_HEIGHT - 2;
							bd = toImageY < 0 && intY > 0;
							be = toImageZ > 0 && intZ < WORLD_DEPTH - 2;
							bf = toImageZ < 0 && intZ > 0;
							if(blocks[intX][intY][intZ] != 0 || (bc && ((be && (blocks[intX][intY + 1][intZ + 1] != 0)) || (bf && (blocks[intX][intY + 1][intZ - 1] != 0)) || blocks[intX][intY + 1][intZ] != 0)) || (bd && ((be && (blocks[intX][intY - 1][intZ + 1] != 0)) || (bf && (blocks[intX][intY - 1][intZ - 1])) != 0 || blocks[intX][intY - 1][intZ] != 0)) || (be && (blocks[intX][intY][intZ + 1])) != 0 || (bf && (blocks[intX][intY][intZ - 1])) != 0){
								goto found;
							}
							if(toImageX > 0 && intX < WORLD_WIDTH - 2 && (blocks[intX + 1][intY][intZ] != 0 || (bc && ((be && (blocks[intX + 1][intY + 1][intZ + 1])) != 0 || (bf && (blocks[intX + 1][intY + 1][intZ - 1])) != 0 || blocks[intX + 1][intY + 1][intZ] != 0)) || (bd && ((be && (blocks[intX + 1][intY - 1][intZ + 1])) != 0 || (bf && (blocks[intX + 1][intY - 1][intZ - 1])) != 0 || blocks[intX + 1][intY - 1][intZ] != 0)) || (be && (blocks[intX + 1][intY][intZ + 1] != 0)) || (bf && (blocks[intX + 1][intY][intZ - 1])) != 0)){
								goto found;
							}
							if(toImageX < 0 && intX > 0 && (blocks[intX - 1][intY][intZ] != 0 || (bc && ((be && (blocks[intX - 1][intY + 1][intZ + 1])) != 0 || (bf && (blocks[intX - 1][intY + 1][intZ - 1])) != 0 || blocks[intX - 1][intY + 1][intZ] != 0)) || (bd && ((be && (blocks[intX - 1][intY - 1][intZ + 1])) != 0 || (bf && (blocks[intX - 1][intY - 1][intZ - 1])) != 0 || blocks[intX - 1][intY - 1][intZ] != 0)) || (be && (blocks[intX - 1][intY][intZ + 1])) != 0 || (bf && (blocks[intX - 1][intY][intZ - 1])) != 0)){
								goto found;
							}
							goto notfound;
							found:
							if(bb){
								bb = 0;
								stepX -= toImageXn;
								stepY -= toImageYn;
								stepZ -= toImageZn;
							}
							goto afternotfound;
							notfound:
							bb = 1;
							afternotfound:
							;
						}
						if(bb == 0){
							block = blocks[intX][intY][intZ];
							if(block != 0){
								currR = 128;
								currG = 128;
								currB = 128;
								wX = stepX - toImageX;
								wY = stepY - toImageY;
								wZ = stepZ - toImageZ;
								
								if(wY >= intY + 1){
									s1 = (intY + 1 - wY) / toImageY;
									//if(s1 >= 0){
										pixelFX = wX + s1 * toImageX;
										pixelFZ = wZ + s1 * toImageZ;
										if(pixelFX >= intX && pixelFX < intX + 1 && pixelFZ >= intZ && pixelFZ < intZ + 1) {
											#ifdef FLOATING_POINT
											pixelX = (uint8_t)((pixelFX - intX) * 16.0f);
											pixelZ = (uint8_t)((pixelFZ - intZ) * 16.0f);
											#else
											pixelX = (uint8_t)((pixelFX - intX) * 16.0K);
											pixelZ = (uint8_t)((pixelFZ - intZ) * 16.0K);
											#endif
											if(block == 1){
												currR = TEXTURE_stone[pixelX][pixelZ][0];
												currG = TEXTURE_stone[pixelX][pixelZ][1];
												currB = TEXTURE_stone[pixelX][pixelZ][2];
											}else if(block == 2){
												currR = TEXTURE_dirt[pixelX][pixelZ][0];
												currG = TEXTURE_dirt[pixelX][pixelZ][1];
												currB = TEXTURE_dirt[pixelX][pixelZ][2];
											}else if(block == 3){
												currR = TEXTURE_grass[pixelX][pixelZ][0];
												currG = TEXTURE_grass[pixelX][pixelZ][1];
												currB = TEXTURE_grass[pixelX][pixelZ][2];
											}else if(block == 4){
												currR = TEXTURE_planks[pixelX][pixelZ][0];
												currG = TEXTURE_planks[pixelX][pixelZ][1];
												currB = TEXTURE_planks[pixelX][pixelZ][2];
											}else if(block == 5){
												currR = TEXTURE_log_top[pixelX][pixelZ][0];
												currG = TEXTURE_log_top[pixelX][pixelZ][1];
												currB = TEXTURE_log_top[pixelX][pixelZ][2];
											}else if(block == 6){
												currR = TEXTURE_leaves[pixelX][pixelZ][0];
												currG = TEXTURE_leaves[pixelX][pixelZ][1];
												currB = TEXTURE_leaves[pixelX][pixelZ][2];
											}
											break;
										}
									//}
								}
								if(wZ <= intZ){
									s1 = -(intZ - wZ) / -toImageZ;
									//if(s1 >= 0){
										pixelFX = wX + s1 * toImageX;
										pixelFY = wY + s1 * toImageY;
										if(pixelFX >= intX && pixelFX < intX + 1 && pixelFY >= intY && pixelFY < intY + 1) {
											#ifdef FLOATING_POINT
											pixelX = (uint8_t)((pixelFX - intX) * 16.0f);
											pixelY = (uint8_t)((pixelFY - intY) * 16.0f);
											#else
											pixelX = (uint8_t)((pixelFX - intX) * 16.0K);
											pixelY = (uint8_t)((pixelFY - intY) * 16.0K);
											#endif
											pixelY = 15 - pixelY;
											if(block == 1){
												currR = TEXTURE_stone[pixelY][pixelX][0];
												currG = TEXTURE_stone[pixelY][pixelX][1];
												currB = TEXTURE_stone[pixelY][pixelX][2];
											}else if(block == 2){
												currR = TEXTURE_dirt[pixelY][pixelX][0];
												currG = TEXTURE_dirt[pixelY][pixelX][1];
												currB = TEXTURE_dirt[pixelY][pixelX][2];
											}else if(block == 3){
												currR = TEXTURE_grass_side[pixelY][pixelX][0];
												currG = TEXTURE_grass_side[pixelY][pixelX][1];
												currB = TEXTURE_grass_side[pixelY][pixelX][2];
											}else if(block == 4){
												currR = TEXTURE_planks[pixelY][pixelX][0];
												currG = TEXTURE_planks[pixelY][pixelX][1];
												currB = TEXTURE_planks[pixelY][pixelX][2];
											}else if(block == 5){
												currR = TEXTURE_log_side[pixelY][pixelX][0];
												currG = TEXTURE_log_side[pixelY][pixelX][1];
												currB = TEXTURE_log_side[pixelY][pixelX][2];
											}else if(block == 6){
												currR = TEXTURE_leaves[pixelY][pixelX][0];
												currG = TEXTURE_leaves[pixelY][pixelX][1];
												currB = TEXTURE_leaves[pixelY][pixelX][2];
											}
											break;
										}
									//}
								}
								if(wZ >= intZ + 1){
									s1 = (intZ + 1 - wZ) / toImageZ;
									//if(s1 >= 0){
										pixelFX = wX + s1 * toImageX;
										pixelFY = wY + s1 * toImageY;
										if(pixelFX >= intX && pixelFX < intX + 1 && pixelFY >= intY && pixelFY < intY + 1) {
											#ifdef FLOATING_POINT
											pixelX = (uint8_t)((pixelFX - intX) * 16.0f);
											pixelY = (uint8_t)((pixelFY - intY) * 16.0f);
											#else
											pixelX = (uint8_t)((pixelFX - intX) * 16.0K);
											pixelY = (uint8_t)((pixelFY - intY) * 16.0K);
											#endif
											pixelY = 15 - pixelY;
											if(block == 1){
												currR = TEXTURE_stone[pixelY][pixelX][0];
												currG = TEXTURE_stone[pixelY][pixelX][1];
												currB = TEXTURE_stone[pixelY][pixelX][2];
											}else if(block == 2){
												currR = TEXTURE_dirt[pixelY][pixelX][0];
												currG = TEXTURE_dirt[pixelY][pixelX][1];
												currB = TEXTURE_dirt[pixelY][pixelX][2];
											}else if(block == 3){
												currR = TEXTURE_grass_side[pixelY][pixelX][0];
												currG = TEXTURE_grass_side[pixelY][pixelX][1];
												currB = TEXTURE_grass_side[pixelY][pixelX][2];
											}else if(block == 4){
												currR = TEXTURE_planks[pixelY][pixelX][0];
												currG = TEXTURE_planks[pixelY][pixelX][1];
												currB = TEXTURE_planks[pixelY][pixelX][2];
											}else if(block == 5){
												currR = TEXTURE_log_side[pixelY][pixelX][0];
												currG = TEXTURE_log_side[pixelY][pixelX][1];
												currB = TEXTURE_log_side[pixelY][pixelX][2];
											}else if(block == 6){
												currR = TEXTURE_leaves[pixelY][pixelX][0];
												currG = TEXTURE_leaves[pixelY][pixelX][1];
												currB = TEXTURE_leaves[pixelY][pixelX][2];
											}
											break;
										}
									//}
								}
								if(wY <= intY){
									s1 = -(intY - wY) / -toImageY;
									//if(s1 >= 0){
										pixelFX = wX + s1 * toImageX;
										pixelFZ = wZ + s1 * toImageZ;
										if(pixelFX >= intX && pixelFX < intX + 1 && pixelFZ >= intZ && pixelFZ < intZ + 1) {
											#ifdef FLOATING_POINT
											pixelX = (uint8_t)((pixelFX - intX) * 16.0f);
											pixelZ = (uint8_t)((pixelFZ - intZ) * 16.0f);
											#else
											pixelX = (uint8_t)((pixelFX - intX) * 16.0K);
											pixelZ = (uint8_t)((pixelFZ - intZ) * 16.0K);
											#endif
											pixelX = 15 - pixelX;
											if(block == 1){
												currR = TEXTURE_stone[pixelX][pixelZ][0];
												currG = TEXTURE_stone[pixelX][pixelZ][1];
												currB = TEXTURE_stone[pixelX][pixelZ][2];
											}else if(block == 2){
												currR = TEXTURE_dirt[pixelX][pixelZ][0];
												currG = TEXTURE_dirt[pixelX][pixelZ][1];
												currB = TEXTURE_dirt[pixelX][pixelZ][2];
											}else if(block == 3){
												currR = TEXTURE_dirt[pixelX][pixelZ][0];
												currG = TEXTURE_dirt[pixelX][pixelZ][1];
												currB = TEXTURE_dirt[pixelX][pixelZ][2];
											}else if(block == 4){
												currR = TEXTURE_planks[pixelX][pixelZ][0];
												currG = TEXTURE_planks[pixelX][pixelZ][1];
												currB = TEXTURE_planks[pixelX][pixelZ][2];
											}else if(block == 5){
												currR = TEXTURE_log_top[pixelX][pixelZ][0];
												currG = TEXTURE_log_top[pixelX][pixelZ][1];
												currB = TEXTURE_log_top[pixelX][pixelZ][2];
											}else if(block == 6){
												currR = TEXTURE_leaves[pixelX][pixelZ][0];
												currG = TEXTURE_leaves[pixelX][pixelZ][1];
												currB = TEXTURE_leaves[pixelX][pixelZ][2];
											}
											break;
										}
									//}
								}
								if(wX >= intX + 1){
									s1 = (intX + 1 - wX) / toImageX;
									//if(s1 >= 0){
										pixelFZ = wZ + s1 * toImageZ;
										pixelFY = wY + s1 * toImageY;
										if(pixelFZ >= intZ && pixelFZ < intZ + 1 && pixelFY >= intY && pixelFY < intY + 1) {
											#ifdef FLOATING_POINT
											pixelZ = (uint8_t)((pixelFZ - intZ) * 16.0f);
											pixelY = (uint8_t)((pixelFY - intY) * 16.0f);
											#else
											pixelZ = (uint8_t)((pixelFZ - intZ) * 16.0K);
											pixelY = (uint8_t)((pixelFY - intY) * 16.0K);
											#endif
											pixelY = 15 - pixelY;
											if(block == 1){
												currR = TEXTURE_stone[pixelY][pixelZ][0];
												currG = TEXTURE_stone[pixelY][pixelZ][1];
												currB = TEXTURE_stone[pixelY][pixelZ][2];
											}else if(block == 2){
												currR = TEXTURE_dirt[pixelY][pixelZ][0];
												currG = TEXTURE_dirt[pixelY][pixelZ][1];
												currB = TEXTURE_dirt[pixelY][pixelZ][2];
											}else if(block == 3){
												currR = TEXTURE_grass_side[pixelY][pixelZ][0];
												currG = TEXTURE_grass_side[pixelY][pixelZ][1];
												currB = TEXTURE_grass_side[pixelY][pixelZ][2];
											}else if(block == 4){
												currR = TEXTURE_planks[pixelY][pixelZ][0];
												currG = TEXTURE_planks[pixelY][pixelZ][1];
												currB = TEXTURE_planks[pixelY][pixelZ][2];
											}else if(block == 5){
												currR = TEXTURE_log_side[pixelY][pixelZ][0];
												currG = TEXTURE_log_side[pixelY][pixelZ][1];
												currB = TEXTURE_log_side[pixelY][pixelZ][2];
											}else if(block == 6){
												currR = TEXTURE_leaves[pixelY][pixelZ][0];
												currG = TEXTURE_leaves[pixelY][pixelZ][1];
												currB = TEXTURE_leaves[pixelY][pixelZ][2];
											}
											break;
										}
									//}
								}
								if(wX <= intX){
									s1 = -(intX - wX) / -toImageX;
									//if(s1 >= 0){
										pixelFZ = wZ + s1 * toImageZ;
										pixelFY = wY + s1 * toImageY;
										if(pixelFZ >= intZ && pixelFZ < intZ + 1 && pixelFY >= intY && pixelFY < intY + 1) {
											#ifdef FLOATING_POINT
											pixelZ = (uint8_t)((pixelFZ - intZ) * 16.0f);
											pixelY = (uint8_t)((pixelFY - intY) * 16.0f);
											#else
											pixelZ = (uint8_t)((pixelFZ - intZ) * 16.0K);
											pixelY = (uint8_t)((pixelFY - intY) * 16.0K);
											#endif
											pixelY = 15 - pixelY;
											if(block == 1){
												currR = TEXTURE_stone[pixelY][pixelZ][0];
												currG = TEXTURE_stone[pixelY][pixelZ][1];
												currB = TEXTURE_stone[pixelY][pixelZ][2];
											}else if(block == 2){
												currR = TEXTURE_dirt[pixelY][pixelZ][0];
												currG = TEXTURE_dirt[pixelY][pixelZ][1];
												currB = TEXTURE_dirt[pixelY][pixelZ][2];
											}else if(block == 3){
												currR = TEXTURE_grass_side[pixelY][pixelZ][0];
												currG = TEXTURE_grass_side[pixelY][pixelZ][1];
												currB = TEXTURE_grass_side[pixelY][pixelZ][2];
											}else if(block == 4){
												currR = TEXTURE_planks[pixelY][pixelZ][0];
												currG = TEXTURE_planks[pixelY][pixelZ][1];
												currB = TEXTURE_planks[pixelY][pixelZ][2];
											}else if(block == 5){
												currR = TEXTURE_log_side[pixelY][pixelZ][0];
												currG = TEXTURE_log_side[pixelY][pixelZ][1];
												currB = TEXTURE_log_side[pixelY][pixelZ][2];
											}else if(block == 6){
												currR = TEXTURE_leaves[pixelY][pixelZ][0];
												currG = TEXTURE_leaves[pixelY][pixelZ][1];
												currB = TEXTURE_leaves[pixelY][pixelZ][2];
											}
											break;
										}
									//}
								}
							
								break;
							}
						}
					}
					if(bb){
						stepX += toImageXn;
						stepY += toImageYn;
						stepZ += toImageZn;
					}else{
						stepX += toImageX;
						stepY += toImageY;
						stepZ += toImageZ;
					}
					stepCntr++;
				}
				
				/*dataBuffer[(j * width) + i] = currR;
				dataBuffer[(j * width) + i + 1] = currG;
				dataBuffer[(j * width) + i + 2] = currB;*/
				bufSetPixel(screen->pixels, i ,j, SDL_MapRGB(screen->format, currR, currG, currB));
			}
		}
		SDL_Flip(screen);
		
		if (real_FPS > SDL_GetTicks()-start)
		{
			SDL_Delay(real_FPS-(SDL_GetTicks()-start));
		}
	}
	
	SDL_FreeSurface(screen);
	SDL_Quit();
}
