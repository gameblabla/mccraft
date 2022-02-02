# MCTest

![](screenshot.png)

This originally came from this forum thread :
https://www.avrfreaks.net/forum/simple-3d-graphics-atmega-microcontroller

TheGhastModding made a minecraft clone of sorts for ATMega.
His code basically just outputs the 24-bits buffer to a BMP file.

This version was modified so it would work on a PC and fix some building errors.
The original program was designed to be run from an 8-bits CPU, the ATmega32.

# My plans

Crafti is unfortunately very memory intensive and on top of that, 
has C++ requirements that are very annoying.
This prevents the game from being ported to other platforms.

# TODO

- Make sure the fixed point code can work without the fixed point draft from GCC
- Port this to SDL 1.2
- Add 8-bits, 16-bits and 32-bits renderers
- Add camera controls to the game
- Eventually add collisions
- Being able to place a block in the world
