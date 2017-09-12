#ifndef GBA_H
#define GBA_H

#include <string.h>
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160
//Display Control flags
// tile modes 
#define MODE0 0x00
#define MODE1 0x01
#define MODE2 0x02
// bitmap modes
#define MODE3 0x03
#define MODE4 0x04

// Max number of colors in palette block
#define PALETTE_SIZE 256





//------------------------------------IO Registers---------------------------------------
/* 
	VRAM (video ram) is a pointer to base memory addr of where graphics data is stored
	- vram spans from 06000000 to 06017FFF
	- size of vram is 64 kb
 	- size of address is 2 bytes 
	- short used since ports are 16 bits large
*/
volatile unsigned short* vram = (volatile unsigned short*) 0x06000000;

/*
  	Display control register is 4 bytes and is used to control graphic modes
	Bits	|F	 |E	  |D   |C	| B	  | A	 | 9	| 8   | 7  | 6   | 5  | 4  | 3  |2   1   0
	Field	|OW  |W1  |W0  |Obj | BG3 |	BG2  | BG1  | BG0 |	FB | OM  | HB | PS | GB | Mode
 	
*/
volatile unsigned long* display_control = (volatile unsigned long*) 0x04000000; //note 4 bytes is long addr
/*
  display status register is 2 bytes and is used to read display statuses
	F E D C | B A 9 8 | 7 6 5 4  3 2 1 0 
	T T T T | T T T T | X X Y H  V Z G W 
*/
volatile unsigned short* display_status = (volatile unsigned short*) 0x04000004;
/*
	Read-only location of y location of display.  
	GBA display height is 160 and is followed by 68 lines of vblank period. 	
 	Waiting for value to reach 160 (vblank) before updating can sync display to 60 fps (Hz). 
*/
volatile unsigned short* scan_vcount = (volatile unsigned short*) 0x04000006;
/*
	Background control registers (2 bytes so short)

	Bits   |F E  | D    |C B A 9 8 |7   |6	 |5 4	| 3 2	| 1 0
	Field  |Size |Wrap  |SBB 	   |CM  |Mos | 	~ 	| CBB 	| P	
	
	Size:		Regular		Affine 	(tile wxh)
		0 	 	32x32 		16x16
		1 		64x32 	 	32x32
		2 		32x64 	 	64x64
		3 		64x64 	  	128x128
	
	Wrap: if set, wraps background vertically and horizontally on display
	SBB :	Screen Base Block.Values 0-31 base screenblock for map indexing
	CM	: 	Color Mode. 0 = 16 colors (4bpp), 1 = 256 colors (8bpp) 
	Mos : 	If set, enables mosaic effect
	CBB :	Character Base Block.Values 0-31 base character block for tile/character indexing
	P   : 	Determines priority of background (used for draw order 0-3)	

*/
volatile unsigned short* bg0_control = (volatile unsigned short*) 0x4000008;
volatile unsigned short* bg1_control = (volatile unsigned short*) 0x400000a;
volatile unsigned short* bg2_control = (volatile unsigned short*) 0x400000c;
volatile unsigned short* bg3_control = (volatile unsigned short*) 0x400000e;
/*
	Background scroll registers
		Write Only!, determines the offset of the drawing area of the background
*/
volatile short* bg0_x_scroll = (volatile short*) 0x4000010;
volatile short* bg0_y_scroll = (volatile short*) 0x4000012;
volatile short* bg1_x_scroll = (volatile short*) 0x4000014;
volatile short* bg1_y_scroll = (volatile short*) 0x4000016;
volatile short* bg2_x_scroll = (volatile short*) 0x4000018;
volatile short* bg2_y_scroll = (volatile short*) 0x400001a;
volatile short* bg3_x_scroll = (volatile short*) 0x400001c;
volatile short* bg3_y_scroll = (volatile short*) 0x400001e;
/*
	Palettes are used to store all colors used by an image
	- background palette is at 0500:0000h 
 	- sprite palette is at 0500:0200h
	- 0x0200 is 256 bytes
*/
volatile unsigned short* palette_background = (volatile unsigned short*) 0x05000000;
volatile unsigned short* palette_sprite 	= (volatile unsigned short*) 0x05000200;

/* address where sprite image data is stored */
volatile unsigned short* sprite_image_block = (volatile unsigned short*) 0x6010000;
volatile unsigned short* sprite_attribute_block = (volatile unsigned short*) 0x7000000;

/* the button register holds the bits which indicate whether each button has
 * been pressed - this has got to be volatile as well
 */
volatile unsigned short* buttons = (volatile unsigned short*) 0x04000130;


//--------------------------Button Utilities-----------------------------------------
/* this function checks whether a particular button has been pressed */
inline unsigned char button_pressed(unsigned short button) {
    //if the buttons register anded with the button is zero, then button is down
	//return not zero if 0
    return !(*buttons & button);
}
/*
 the bit positions of each button. 1 is shifted into position
*/
#define BUTTON_A (1 << 0)
#define BUTTON_B (1 << 1)
#define BUTTON_SELECT (1 << 2)
#define BUTTON_START (1 << 3)
#define BUTTON_RIGHT (1 << 4)
#define BUTTON_LEFT (1 << 5)
#define BUTTON_UP (1 << 6)
#define BUTTON_DOWN (1 << 7)
#define BUTTON_R (1 << 8)
#define BUTTON_L (1 << 9)

/*-------------------------Bitmap mode utilities --------------------------------------*/
/*
 16-bit int color is bgr ranged 0-32 (5 bits each color)
	Each color is masked to 5 bits and shifted into location	
	: bbbbbgggggrrrrr 
*/
inline unsigned short get_color(unsigned char r, unsigned char g, unsigned char b) {
    return  ((b&0x1f) << 10) | ((g & 0x1f)<<5) | (r & 0x1f);
}
/*	 
	Bitmap mode - put pixel
	place a pixel of color on the screen at row col 
	position (x, y) is accessed at screen[y*w+x] 
*/
inline void put_pixel(int row, int col, unsigned short color) {
	vram[row * SCREEN_WIDTH + col] = color;
}


/* ---------------------- Tiled Mode utilities ------------------------------*/
/* Screen and Character blocks  
	1 block = 512 bytes
	2 blocks = 1024 bytes = 1 kilobyte
	1 kb = 0x0400
	- Screen blocks are 2 kb
		VRAM contains 32 Screen Blocks
	- Character blocks are 16 kb
		VRAM contains 4 Screen Blocks
	Character Block sre used to store background image data (tileset)
	Screen Blocks are used to store map data (tilemap)
	Note:
	Screen and Character blocks are different ways to access chunks of VRAM,
		If character block 0 is used, only screen blocks 8-31 are unused 
		(1 character block is the size 8 screen blocks, 2 vs 16 kb)
	*/

/*
	Returns base address of nth character block (0-3)	
*/
inline unsigned short* char_block(unsigned long block_n){
	//calculate distance from base vram addr
	return (unsigned short* )(0x06000000+block_n*0x4000); //0x4000 = 16 kb
}

/*
	Returns base address of nth screen block (0-31)	
*/
inline unsigned short* screen_block(unsigned long block_n){
	//calculate distance from base vram addr
	return ( unsigned short*)(0x06000000+ block_n*0x800); //0x800 = 2 kb
}

//--------------------------Miscellaneous------------------------------------- 
/* wait for the screen to be in vblank, finished drawing */
inline void vblank_wait( ) {
    /* wait until all 160 lines have been updated */
    while (*scan_vcount >= 160);
	while (*scan_vcount < 160);
}

inline void delay(unsigned int amount) {
    for (int i = 0; i < amount * 10; i++);
}
#endif //GBA_H
