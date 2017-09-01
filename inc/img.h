
#include <gba.h>
#ifndef IMG_H
#define IMG_H

#define SIZE_REG_32x32 0 
#define SIZE_REG_64x32 1
#define SIZE_REG_32x64 2
#define SIZE_REG_64x64 3
#define SIZE_AFF_16x16 0 
#define SIZE_AFF_32x32 1
#define SIZE_AFF_64x64 2
#define SIZE_AFF_128x128 3

/*--------------------------Image Utilities-------------------------------
	load_image and load_map are used to load background information into VRAM
	Once an image and map data are loaded into vram, the backgroud
*/
/*
	Image container

*/
struct image
{
	const unsigned char* data; //array of indices corresponding to tiles in char block
	const unsigned short* palette;
	//dimensions of array
	unsigned long width;
	unsigned long height;	 
};

struct map
{	
	const unsigned short* data; //array of indices corresponding to tiles in char block
	//dimensions of array
	unsigned long width;
	unsigned long height;
};


/*
		loads image palette and data into vram at the nth character block (0-3)
*/
void load_image(struct image image, unsigned long char_block_n)
{
	//load the palette data into memory
	for(int i=0; i < PALETTE_SIZE; i++){
		palette_background[i]= image.palette[i];
	}
	//read data as shorts, vram addr are shorts, 2 bytes
	unsigned short* data = (unsigned short*)image.data;
	//get the address of the character block to use
	unsigned short* block = char_block(char_block_n);
	//image data is an array of chars(1byte) but read as an array of shorts(2bytes). 
	//	(short array is half the length ) so divide char length by 2
	unsigned int len  =((image.width * image.height) / 2);
	for (int i = 0; i < len; i++) {
		block[i] = data[i];
	}
}

/*
	Loads the map from the tiles in the background screen_block
*/

void load_map(struct map map, unsigned long screen_block_n)
{
	unsigned short* block = screen_block(screen_block_n); 
	//loads tile index data into screen block
	unsigned int len  =(map.width * map.height);
	for (int i=0; i < len; i++){
		block[i] = map.data[i];
	}
}


#endif

