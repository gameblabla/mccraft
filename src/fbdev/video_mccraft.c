#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h> 
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include "defines.h"

uint8_t* framebuffer_game;

static uint16_t* restrict buffer_fbdev[1];
static int fbfd = 0, keyboard_fd = 0;
static struct input_event data;

static struct fb_var_screeninfo orig_vinfo;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;

static size_t screensize = 0;

#ifndef _16BIT
#error "Only 16-bits supported for FBDEV at this time"
#endif

int Init_video()
{
	fbfd = open("/dev/fb0", O_RDWR | O_SYNC);
	if (!fbfd) 
	{
		printf("Error: cannot open framebuffer device.\n");
		return 1 ;
	}
	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Error reading variable information.\n");
	}

	// Store for reset (copy vinfo to vinfo_orig)
	memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

	// Change variable info
	vinfo.bits_per_pixel = 16;
	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
		printf("Error : Setting variable information.\n");
	}
	
	// Get fixed screen information
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
		printf("Error reading fixed information.\n");
	}
	
	screensize = (SCREEN_WIDTH*SCREEN_HEIGHT)*2;
	framebuffer_game = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
	ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo); 
}

void Start_video()
{

}

void Refresh_video()
{
	ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo);
}

void Quit_video()
{

}
