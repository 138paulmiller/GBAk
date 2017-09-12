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
enum sprite_size {
    SPRITE_8_8,
    SPRITE_16_16,
    SPRITE_32_32,
    SPRITE_64_64,
    SPRITE_16_8,
    SPRITE_32_8,
    SPRITE_32_16,
    SPRITE_64_32,
    SPRITE_8_16,
    SPRITE_8_32,
    SPRITE_16_32,
    SPRITE_32_64
};


#define SPRITE_NUM 128

// sprite flags used in display control
#define SPRITE_MAP_2D 0x0
#define SPRITE_MAP_1D 0x40
#define SPRITE_ENABLE 0x1000

/*
Sprite Image
	GBA allows for only one sprive source image to be loaded at a time
*/
struct sprite_img
{
	const unsigned char* data;
	const unsigned short* palette;
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
	Field	|Palette 	| Priority| Tile Index (Offset)
	
	*/	
	unsigned short attr2;
	/*
	Affine Transformations, not used
	*/
	unsigned short attr3;
} sprites[SPRITE_NUM]; //all possible sprites
int sprite_index = 0;

/*
	Loads sprite image into vram. 
*/
void sprite_load_img(struct sprite_img img){
	memcpy((unsigned short*) palette_sprite, (unsigned short*) img.palette, PALETTE_SIZE);

    /* load the image into char block 0 */
    memcpy((unsigned short*) sprite_image_block, (unsigned short*) img.data, (img.width * img.height) / 2);
/*
	for(int i=0; i< PALETTE_SIZE; i++){
		palette_sprite[i] = img.palette[i];
	}
	unsigned short* data = (unsigned short*) img.data;
	//divide size by 2 since data is char(1byte) format and is being read as short(2 bytes)
	unsigned int height = (img.height)/2; 
	int i =0;
	unsigned int frame_width = img.width/img.frames;
	//load frame into vram
	for(int frame=0; frame < img.width; frame+=frame_width){
		for(int col = 0; col < frame_width; col++){	
			for(int row = 0; row < height; row++){	
				sprite_image_block[i++] = data[row*img.width+col+frame];
			}
		}
	}
*/
}


/*
	Sets sprite attribute in vram 
*/
struct sprite* sprite_make(int x, int y, enum sprite_size size,
						   int h_flip, int v_flip, int tile_offset, int priority)

{
	static unsigned int sprite_index = 0;
	struct sprite* sprite = 0;
	if(sprite_index < SPRITE_NUM)
	{
		int size_flag, shape_flag;
    	switch (size) {
	        case SPRITE_8_8:   size_flag = 0; shape_flag = 0; break;
	        case SPRITE_16_16: size_flag = 1; shape_flag = 0; break;
	        case SPRITE_32_32: size_flag = 2; shape_flag = 0; break;
	        case SPRITE_64_64: size_flag = 3; shape_flag = 0; break;
	        case SPRITE_16_8:  size_flag = 0; shape_flag = 1; break;
	        case SPRITE_32_8:  size_flag = 1; shape_flag = 1; break;
	        case SPRITE_32_16: size_flag = 2; shape_flag = 1; break;
	        case SPRITE_64_32: size_flag = 3; shape_flag = 1; break;
	        case SPRITE_8_16:  size_flag = 0; shape_flag = 2; break;
	        case SPRITE_8_32:  size_flag = 1; shape_flag = 2; break;
	        case SPRITE_16_32: size_flag = 2; shape_flag = 2; break;
	        case SPRITE_32_64: size_flag = 3; shape_flag = 2; break;
    	}
		sprite = &sprites[sprite_index++];
		sprite->attr0 = y //8 bits for y value
				   	|(0 << 8)   //affine
				   	|(0 << 10)	//effect
				   	|(0 << 12) //mosaic
				  	|(COLOR_MODE << 13) //color mode from GBA.h
					|((shape_flag & 0x03) << 14); //mask 2 bits of shape
		sprite->attr1 = x //9 bits
						| (0 << 9)           // not affine flag 
                         |   (h_flip << 12)          // horizontal flip flag 
                         |   (v_flip << 13)          // vertical flip flag 
                         |   (size_flag << 14);  // size 

    	sprite->attr2 = 	tile_offset   // tile index 
                         |   (priority << 10) //priority 
                         |   (0 << 12);          // 16 color palette
	
	}
	return sprite;
}

// set sprite pos
void sprite_set_pos(struct sprite* sprite, int x, int y) {
    // clear lower 8 bits of attr0
    sprite->attr0 &= 0xff00;
    // set the new y pos in lower 8 bits by clearing the upper bits 8 of x
    sprite->attr0 |= (y & 0xff);
    // clear lower 9 bits of attr1
    sprite->attr1 &= 0xfe00;
    // set the new x pos in lower 9 bits by clearing the upper bits 7 of x
    sprite->attr1 |= (x & 0x1ff);
}


// update the tile offset of a sprite
void sprite_set_offset(struct sprite* sprite, int offset) {
    /* clear the old offset */
    sprite->attr2 &= 0xfc00;
    /* apply the new one */
    sprite->attr2 |= (offset & 0x03ff);
}

//Move sprites position by dx, dy
void sprite_move_by(struct sprite* sprite, int dx, int dy) {
    // get current pos
    int x = sprite->attr1 & 0x1ff;
    int y = sprite->attr0 & 0xff;
    //add by deltas
    sprite_set_pos(sprite, x + dx, y + dy);
}

// clear all sprites data 
void sprite_clear_all() {
    /* clear the index counter */
    sprite_index = 0;
    /* move all sprites offscreen to hide them */
    for(int i = 0; i < SPRITE_NUM; i++) {
        sprites[i].attr0 = (SCREEN_HEIGHT & 0xff);
        sprites[i].attr1 = (SCREEN_WIDTH& 0x1ff);
    }
}
void sprite_update_all() {
    memcpy((unsigned short*) sprite_attribute_block, (unsigned short*) sprites, SPRITE_NUM * 4);
}
#endif






