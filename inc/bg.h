
#include <gba.h>
#ifndef BG_H
#define BG_H

/*
	Contains Background and Background Image Utilities

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

// background enable flags used in display control
#define BG0 0x100
#define BG1 0x200
#define BG2 0x400
#define BG3 0x800

//256 color mode
#define COLOR_MODE 1 
//Bacground wrap flags
#define BG_WRAP 1
#define BG_NO_WRAP 0 


/*--------------------------Background Utilities---------------------------------
	load_image and load_map are used to load background information into VRAM
	Backgrounds read from the character blocks and screen blocks used by loaded 
	images and maps.
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

struct bg_map
{	
	const unsigned short* data; //array of indices corresponding to tiles in char block
	//dimensions of array
	unsigned int width;
	unsigned int height;
};

/*
	Set_Background 
		Sets the background of the display using image and map data loaded 
		in vram character and screen blocks
	bg_control		: the control register to write to(bg0_control - bg3_control)
	char_block_n	: the character block to use (0-3), specifies source images (tileset)
	screen_block_n	: screen block to use (0-3), specifies map indices (tilemap)
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
	load_image
		loads image palette and data into vram at the nth character block (0-3)
*/
inline void bg_load(struct bg bg, unsigned int char_block_n)
{
	//load the palette data into memory
	for(int i=0; i < PALETTE_SIZE; i++){
		palette_background[i]= bg.palette[i];
	}
	//read image data as shorts, vram addr are shorts, 2 bytes
	unsigned short* data = (unsigned short*)bg.img_data;
	//get the address of the character block to use
	unsigned short* block = char_block(char_block_n);
	//image data is an array of chars(1byte) but read as an array of shorts(2bytes). 
	//	(short array is half the length ) so divide char length by 2
	unsigned int len  =((bg.width * bg.height) / 2);
	for (int i = 0; i < len; i++) {
		block[i] = data[i];
	}
	//memcpy(palette_background, image.palette, PALETTE_SIZE*sizeof(short));
	//memcpy(char_block(char_block_n), (unsigned short*)image.data, (image.width * image.height)*sizeof(short));
}

/*
	Loads the map from the tiles in the background screen_block
*/

inline void bg_map_load(struct bg_map map, unsigned int screen_block_n)
{

	unsigned short* block = screen_block(screen_block_n); 
	//loads tile index data into screen block
	unsigned int len  =(map.width * map.height);
	for (int i=0; i < len; i++){
		block[i] = map.data[i];
	}
	//memcpy((unsigned short*) screen_block(screen_block_n), map.data, (map.width * map.height));

}

#endif

