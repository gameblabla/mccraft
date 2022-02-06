#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "video_mccraft.h"
#include "defines.h"
#include "textures.h"

//The World
static uint8_t blocks[WORLD_WIDTH][WORLD_HEIGHT][WORLD_DEPTH];

	//Working variables
#ifdef FLOATING_POINT
static float cosX,cosY,sinX,sinY;
static float toImageX,toImageY,toImageZ,toImageXn,toImageYn,toImageZn;
static float length;
static float stepX,stepY,stepZ;
static float s1;
static float pixelFX,pixelFY,pixelFZ;
static float wX,wY,wZ;
#else
static accum cosX,cosY,sinX,sinY;
static accum toImageX,toImageY,toImageZ,toImageXn,toImageYn,toImageZn;
static accum length;
static accum stepX,stepY,stepZ;
static accum s1;
static accum pixelFX,pixelFY,pixelFZ;
static accum wX,wY,wZ;
#endif
static signed char intX,intY,intZ;
static signed char prevIntX,prevIntY,prevIntZ;
static uint8_t bb;
static uint8_t block;
static uint8_t pixelX,pixelY,pixelZ;
static uint8_t bc,bd,be,bf;

#ifdef FLOATING_POINT
float x,y,z = 0;
float rotX,rotY = 0.0f;
#else
accum x,y,z = 0;
accum rotX,rotY = 0;
#endif


#ifdef FLOATING_POINT
static float toRadians(float deg){
	return deg * 0.017453292519943295f;
}
#else
static accum toRadians(accum deg){
	return deg * 0.017453292519943295K;
}
#endif

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

void Rendering_World()
{
	uint16_t stepCntr = 0;
	while(1)
	{
					prevIntX = intX;
					prevIntY = intY;
					prevIntZ = intZ;
					intX = (signed char)stepX;
					intY = (signed char)stepY;
					intZ = (signed char)stepZ;
					if(prevIntX != intX || prevIntY != intY || prevIntZ != intZ) 
					{
						if(intX >= WORLD_WIDTH || intX < 0 || intY >= WORLD_HEIGHT || intY < 0 || intZ >= WORLD_DEPTH || intZ < 0) {
							#ifdef FLOATING_POINT
							if(toImageY < -0.125f / (float)STEP_RESOLUTION) {
								color_texture = VOID_RGB_COLOR;
							}
							#else
							if(toImageY < -0.125K / (accum)STEP_RESOLUTION) {
								color_texture = VOID_RGB_COLOR;
							}
							#endif
							break;
						}
						if(bb || stepCntr >= STEP_RESOLUTION) {
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
								color_texture = EMPTY_RGB_COLOR;
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
											color_texture = TEXTURE_ARRAY[ PROCESS_BLOCK_MULT + pixelX + (pixelZ * 16)];
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
											color_texture = TEXTURE_ARRAY[ PROCESS_BLOCK_MULT + pixelY + (pixelX * 16)];
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
											color_texture = TEXTURE_ARRAY[ PROCESS_BLOCK_MULT + pixelY + (pixelX * 16)];
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
											color_texture = TEXTURE_ARRAY[ PROCESS_BLOCK_MULT + pixelX + (pixelZ * 16)];
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
											color_texture = TEXTURE_ARRAY[ PROCESS_BLOCK_MULT + pixelY + (pixelZ * 16)];
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
											color_texture = TEXTURE_ARRAY[ PROCESS_BLOCK_MULT + pixelY + (pixelZ * 16)];
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
}

void Init_Pos()
{
	//Player position
	#ifdef FLOATING_POINT
	y = 9;
	/*rotX = 4.2300005f;
	rotY = 99.70001f;
	x = 1.7884214f;
	z = 9.30016f;*/
	rotX = -0.0900002f;
	rotY = 47.365f;
	x = 0.404446f;
	z = 0.22908628f;
	#else
	y = 9;
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
}

void World_loop()
{
	int j ,i;
	#ifdef FLOATING_POINT
	rotY+= 1.1f;
	#else
	rotY+= 1.1K;
	#endif
	cosX = cos(toRadians(rotX));
	cosY = cos(toRadians(rotY));
	sinX = sin(toRadians(rotX));
	sinY = sin(toRadians(rotY));
		
	for(j = SCREEN_HEIGHT - 1; j >= 0; j--)
	{
		for( i = 0; i < SCREEN_WIDTH; i++)
		{
			//Raytrace!
			#ifdef FLOATING_POINT
			toImageX = ((float)i / (float)SCREEN_WIDTH - 0.5f);
			toImageY = -((float)j / (float)SCREEN_WIDTH - 0.5f);
			toImageZ = 1.0f;
			#else
			toImageX = ((accum)i / (accum)SCREEN_WIDTH - 0.5K);
			toImageY = -((accum)j / (accum)SCREEN_WIDTH - 0.5K);
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
			toImageX /= STEP_RESOLUTION;
			toImageY /= STEP_RESOLUTION;
			toImageZ /= STEP_RESOLUTION;
			bb = 1;
				
			stepX = x;
			stepY = y;
			stepZ = z;
			intX = (signed char)x + 1;
			intY = (signed char)y;
			intZ = (signed char)z;
			color_texture = SKY_RGB_COLOR;
			Rendering_World();
			bufSetPixel(screen->pixels, i ,j, color_texture);
		}
	}	
}
