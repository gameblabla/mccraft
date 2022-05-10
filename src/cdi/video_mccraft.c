#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "video_mccraft.h"
#include "defines.h"


int Init_video()
{
	initVideo();
	initGraphics();
}

void Start_video()
{
}

void Refresh_video()
{
	swapImageBuffer();
}

void Quit_video()
{
}
