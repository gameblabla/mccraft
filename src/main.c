/*
 * MCTest.c
 *
 * Created: 2/23/2019 4:41:28 PM
 *  Author: lucah
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "video_mccraft.h"
#include "defines.h"
#include "game.h"

int main(int argc, char* argv[])
{
	Init_Pos();
	Init_video();
	GenerateWorld();

	while(1)
	{
		Start_video();
		
		World_loop();

		Refresh_video();
	}
	Quit_video();
}
