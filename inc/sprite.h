#ifndef SPRITE_H
#define SPRITE_H
#include <gba.h>


/*				size
			0		1		2		3
shape	0	8x8		16x16	32x32	64x64
		1	16x8	32x8	32x16	64x32
		2	8x16	8x32	16x32	32x64
*/

//shapes
#define SPRITE_REG 0
#define SPRITE_WIDE 1
#define SPRITE_TALL 2
//sizes
#define SPRITE_SMALL 0
#define SPRITE_MEDIUM 1
#define SPRITE_LARGE 2
#define SPRITE_XLARGE 3

#define SPRITE_NUM 128
//all possible sprites
struct sprite_attr sprites[SPRITE_NUM];

/*
Sprite Image
	GBA allows for only one sprive source image to be loaded at a time
*/
struct sprite_img
{
	unsigned char* data;
	unsigned short* palette;
	unsigned int width; 
	unsigned int height; 
	unsigned int frames;//widthxheight of each frame is width/framesxheight
};

/*
 	GBA allows for 128 different Sprites each with 4 attribute registers.
	Sprite reads image data from image loaded into sprite_image memory.
	
*/
struct sprite
{
	/* Attribute 0
	Bits	|15 14 |13		   |12		| 11 10	 |9 8	 | 7 6 5 4 3 2 1 0
	Field	|Shape |Color Mode |Mosaic	| Effect |Affine | Y pos

	Effect: alpha blending or masking. 
	*/
	unsigned short attr0;
	/* Attribute 1
	Bits	|15 14 |13		 |12		| 11 10	9 | 8 7 6 5 4 3 2 1 0
	Field	|Size  |V Flip	 |H Flip	| ~	      | X pos
	*/	
	unsigned short attr1;

	/* Attribute 2
	Bits	|15 14 13 12| 11 10	  | 9  8 7 6 5 4 3 2 1 0
	Field	|Palette 	| Priority| Tile Index
	
	*/	
	unsigned short attr2;
	/*
	Affine Transformations, not used
	*/
	unsigned short attr3;
};
/*
	Loads sprite image into vram. 
*/
void load_sprite_img(struct sprite_img img){
	for(int i=0; i< PALETTE_SIZE; i++){
		palette_sprite[i] = sprite.palette[i];
	}
	unsigned short* data = (unsigned short*) sprite.data;
	//divide size by 2 since data is char(1byte) format and is being read as short(2 bytes)
	unsigned int height = (sprite.height)/2; 
	int i =0;
	unsigned int frame_width = sprite.width/frames;
	//load frame into vram
	for(int frame=0; frame < sprite.width; frame+=frame_width){
		for(int col = 0; col < frame_width; row++){	
			for(int row = 0; row < height; row++){	
				sprite_image_block[i++] = data[row*width+col+frame];
			}
		}
	}
}

/*
	Sets sprite attribute in vram 
*/
struct sprite* make_sprite(int x, int y, unsigned char size, unsigned char shape,
						   int horizontal_flip, int vertical_flip, int tile_index, int priority)

{
	static unsigned int sprite_index = 0;

	if(sprite_index < SPRITE_NUM)
	{
		struct sprite* sprite;
		sprite->attr0 = (y& 0x0ff) //8 bits
				   	|(0 << 8)   //affine
				   	|(0 << 10)	//effect
				   	|(0 << 12) //mosaic
				  	|(COLOR_MODE << 13)
					|((shape & 0x03) << 14); //mask 2 bits of shape
		sprite->attr1 = (x& 0x01ff) //9 bits
					// |Size	V Flip	H Flip
					//    14   |13	   |12
		
	}
	return &sprites[sprite_index++];
}
#endif






