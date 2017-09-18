#ifndef UTIL_H
#define UTIL_H


//Screen size
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

//Images in memory are represented as 8x8 tiles 
#define TILE_WIDTH 8
#define TILE_HEIGHT 8
//Display Control flags
// tile modes 
#define MODE0 0x00
#define MODE1 0x01
#define MODE2 0x02
// bitmap modes
#define MODE3 0x03
#define MODE4 0x04

//256 color mode
#define COLOR_MODE 1 
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




//--------------------------Button Utilities-----------------------------------------


/* the button register holds the bits which indicate whether each button has
 * been pressed - this has got to be volatile as well
 */
volatile unsigned short* buttons = (volatile unsigned short*) 0x04000130;

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
		VRAM contains 4 char Blocks
	Character Block sre used to store background image data (tileset)
	Screen Blocks are used to store map data (tilemap)
	Note:
	Screen and Character blocks are different ways to access chunks of VRAM,
	
	NOTE: if Char blocks 0 and 1 are used, screen blocks 16-31 are available!
	
	BLOCKS:
	Char		Screen Blocks
			----------------------------------------
	0		| 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  |
			---------------------------------------
	1		| 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 |
			----------------------------------------
	2		| 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
			----------------------------------------
	3		| 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
			----------------------------------------
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
//--------------------------DMA (Direct Memory Address)------------------------------------- 
/*
	DMA 
		provides a utility to copy 
*/
// flag enables DMA when written to DMA control register 
#define DMA_ENABLE 0x80000000

// DMA transfer size flags
#define DMA_16 0x00000000
#define DMA_32 0x04000000
// DMA register for address of data's source location 
volatile unsigned int* dma_source = ( unsigned int*) 0x40000D4;

// DMA register for address of data's destination 
volatile unsigned int* dma_destination = ( unsigned int*) 0x40000D8;

//  DMA  control register (32- bit)
/*
| 1F | 1E | 1D 1C | 1B | 1A | 19 | 18 17 |16 15 | 14 13 12 11 10 | F E D C B A 9 8 7 6 5 4 3 2 1 0
| En | I  | Tm    | -  | C  | R  | Sa    | Da   | -              | 		Size 
 En - enable flag
 I  - Interrupt request, raises interuupt if set
 Tm - Timing Mode. Specifies when the transfer should start.
    00: immediately
    01: at vblank
    10: at hblank
    11: at each scanline?? (unsure untested) 
 C  - chunk size: if 0 halfword(16bit) else if 1 word(32bit)
 R  - Repeats at every vblank or hblank if timing mode is set to either
 Da - Destination adjustment, address behavior after each transfer
    00: increment after each transfer (default)
    01: decrement after each transfer
    10: none; address is fixed
    11: increment the destination during the transfer, and reset it.

Sa - Source Adjustment. Works just like the two bits for the destination 
		Except 11 is not a valid opcode.
Size - the amount of data to be transfered  
*/
volatile unsigned int* dma_count = ( unsigned int*) 0x40000DC;

/* copy data using DMA format */
void dma16_transfer(unsigned short* dest, const unsigned short* source, int size) {
    *dma_source = (unsigned int) source;
    *dma_destination = (unsigned int) dest;
    *dma_count = size | DMA_16 | DMA_ENABLE;
}
//--------------------------Miscellaneous------------------------------------- 

// wait for the screen to be in vblank
inline void vblank_wait( ) {
    /* wait until all 160 lines have been updated */
    while (*scan_vcount >= 160);
	while (*scan_vcount < 160);
}

inline void delay(unsigned int amount) {
    for (int i = 0; i < amount * 10; i++);
}
#endif //UTIL_H
