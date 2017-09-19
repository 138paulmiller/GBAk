
#ifndef BG_H
#define BG_H
#include <gba/util.h>

/*
	Contains Background Utilities for loading background images and tilemap data

*/
//Bacground size properties (number of tiles)
#define BG_32x32 0 
#define BG_64x32 1
#define BG_32x64 2
#define BG_64x64 3
#define BG_16x16_AFFINE 0 
#define BG_32x32_AFFINE 1
#define BG_64x64_AFFINE 2
#define BG_128x128_AFFINE 3


//Tile size properties
#define TILE_REG_32x32 0 
#define TILE_REG_64x32 1
#define TILE_REG_32x64 2
#define TILE_REG_64x64 3
#define TILE_AFF_16x16 0 
#define TILE_AFF_32x32 1
#define TILE_AFF_64x64 2
#define TILE_AFF_128x128 3

/*
 background enable flags used in display control
Mode:	BG0 	BG1 	BG2 	BG3
-----------------------------------
0 		reg 	reg 	reg 	reg
1 		reg 	reg 	aff 	-
2 		- 		- 		aff 	aff 
*/
#define BG0 0x100
#define BG1 0x200
#define BG2 0x400
#define BG3 0x800

//Bacground wrap flags
#define BG_WRAP 1
#define BG_NO_WRAP 0 


/*
	Palettes are used to store all colors used by an image
	- background palette is at 0500:0000h 
 	- sprite palette is at 0500:0200h
	- 0x0200 is 256 bytes
*/
volatile unsigned short* palette_bg = (volatile unsigned short*) 0x05000000;
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
	SBB :	Screen Base Block.Values 0-31 base screenblock for tilemap indexing
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
volatile short* bg0_scroll_x = (volatile short*) 0x4000010;
volatile short* bg0_scroll_y = (volatile short*) 0x4000012;
volatile short* bg1_scroll_x = (volatile short*) 0x4000014;
volatile short* bg1_scroll_y = (volatile short*) 0x4000016;
volatile short* bg2_scroll_x = (volatile short*) 0x4000018;
volatile short* bg2_scroll_y = (volatile short*) 0x400001a;
volatile short* bg3_scroll_x = (volatile short*) 0x400001c;
volatile short* bg3_scroll_y = (volatile short*) 0x400001e;

/*--------------------------Background Utilities---------------------------------
	load_image and load_tilemap are used to load background information into VRAM
	Backgrounds read from the character blocks and screen blocks used by loaded 
	images and tilemaps.
	Note: Multiple backgrounds can share the same character block (image data).
	
*/
/*
	Image container
		data: array of indices corresponding to colors in palette array
		palette : array of bgr colors used in image (palette size = 256) 
		width and height: dimensions of image data
*/
struct bg
{
	const unsigned char* img_data; 		
	const unsigned short* palette;	
	unsigned int width;
	unsigned int height;	 
};

struct bg_tilemap
{	
	const unsigned short* data; //array of indices corresponding to tiles in char block
	//dimensions of array
	unsigned int width;
	unsigned int height;

};

/*
	Set_Background 
		Sets the background of the display using bg image and tilemap data loaded 
		in vram character and screen blocks
	bg_control		: the control register to write to(bg0_control - bg3_control)
	char_block_n	: the character block to use (0-3), specifies source images (tileset)
	screen_block_n	: screen block to use (0-3), specifies tilemap indices (tilemap)
	size			: size of background to draw, 0-3 value
	priority		: draw order of backgrounds (0-3)
	wrap			: if set, vertical and horizontal background wrapped on display
	mosaic			: if set, adds mosaic effect	
*/

inline void bg_set(volatile unsigned short *bg_control, unsigned int char_block_n, unsigned int screen_block_n,
							unsigned short size, unsigned short priority, unsigned short wrap)
{
	*bg_control = priority 
			  | (char_block_n << 2)     
			  | (0 << 6) //no mosaic effect  
			  | (COLOR_MODE << 7) 
			  | (screen_block_n << 8)
			  | (wrap << 13) 
			  | (size << 14);   
}

/*
bg_load_palette
	loads palette into bg_palette for tilemap and bg image to use
*/
inline void bg_load_palette(const unsigned short* palette)
{
	//palette data as shorts instead of chars since palette is char array,
	//  shorts = 2 chars
	dma16_transfer((unsigned short*)palette_bg, palette, PALETTE_SIZE*sizeof(short));
}

/*
	load_image
		loads bg image and palette data into vram at the nth character block (0-3)
*/
inline void bg_load_image(struct bg* bg, unsigned int char_block_n)
{
	//image loaded into char block
	dma16_transfer((unsigned short*)char_block(char_block_n), //dest 
		(unsigned short*)bg->img_data, //source
		(bg->width * bg->height)*sizeof(short)); //scale by sizeof short
}


/*
	Loads the tilemap from the tiles in the background nth screen_block (0-31)	
*/

inline void bg_load_tilemap(struct bg_tilemap* tilemap, unsigned int screen_block_n)
{
	//read tilemap data as shorts, vram addr are shorts, 2 bytes
	//load tilemap data directly into nth screen block
	dma16_transfer((unsigned short*) screen_block(screen_block_n), //dest
				(unsigned short*)tilemap->data,  //source 
				(tilemap->width * tilemap->height)); //bytes

}

/*
	bg_set_offset
		Sets the scroll offset of the background bg_index (0-3)
*/
inline void bg_set_scroll(int bg_index, int offset_x, int offset_y)
{
	switch(bg_index)
	{
		case 0: *bg0_scroll_x = offset_x; 
				*bg0_scroll_y = offset_y;
				break;
		case 1: *bg1_scroll_x = offset_x; 
				*bg1_scroll_y = offset_y;
				break;
		case 2: *bg2_scroll_x = offset_x; 
				*bg2_scroll_y = offset_y;
				break;
		case 3: *bg3_scroll_x = offset_x; 
				*bg3_scroll_y = offset_y;
				break;
		}

}

/*
	get_tile
		returns tile id at the given x,y screen coord
*/
inline unsigned short bg_get_tile(const struct bg_tilemap* tilemap, int x, int y, 
						int scroll_x, int scroll_y) {

    //offset by scroll
    x += scroll_x;
    y += scroll_y;

    // convert from screen coord to tile map index coord 
    // each tile 8x8 so divide screen by 8 to get 1x1 index
    x >>= 3;
    y >>= 3;

    // return the tile at the 2d->1d transformed index
    return tilemap->data[y * tilemap->width + x];
}


/*
bg_set_text
	Draw text on the background at (x, y) with tilemap width w. 
	This is done by first loading text image as a bakground image.
	Each character is a tile that is mapped to the screen.
	Function works similar to the tilemap load function.
*/
inline void bg_set_text(char* str, unsigned int screen_block_n, int w, int x, int y) 
{                    
    //starting 1D index of tilemap
    int index = x * w + y;

    //assuming text image is in ASCII order and ignores control and escape codes 0-32
    int ignore = 32; 
    //pointer to text tilemap screen block being used
    volatile unsigned short* block = (unsigned short*) screen_block(screen_block_n);

    //iterate through each char and map ASCII to image index
    while (*str) {
    	//set char as index to tilemap
        block[index++] = - ignore + *str++ ;
    }   
}


inline void bg_clear_screen_block(unsigned int screen_block_n)
{
	/* clear the text map to be all blanks */
    volatile unsigned short* block = screen_block(screen_block_n);
    for (int i = 0; i < 32 * 32; i++) {
        block[i] = 0;
    }
}
#endif

