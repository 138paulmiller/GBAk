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
 	GBA allows for 128 different Sprites each with 4 attribute registers.
	Sprite reads image data from image loaded into sprite_image memory.
	
*/
struct sprite_attr
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
	Field	|Palette 	| Priority| Tile Index (Frame pos ofset)
	
	*/	
	unsigned short attr2;
	/*
	Affine Transformations, not used
	*/
	unsigned short attr3;

}
//all possible sprite attributes that can be loaded into memory at a time
sprite_attrs[SPRITE_NUM];
unsigned int sprite_attr_index=0;

struct sprite {
    // sprite attribute index
    struct sprite_attr* attr;
	//pos 
    int x, y; 
    //  animation frame 
    int frame;
    // number of frames to wait before next frame */
    int delay;
    //number of frames until we flip 
    int counter;
    // the width of each frame in sprite
    int width;
    //0 if sprite is not moving 
    char is_moving, is_flipped;

};
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
	Loads sprite image into vram. 
*/
void sprite_load_img(struct sprite_img img){
	//load the palette into the sprite palette memory block 
    memcpy_dma16((unsigned short*) palette_sprite, (unsigned short*) img.palette, PALETTE_SIZE);
    //load the image into the sprite image memory block 
    memcpy_dma16((unsigned short*) sprite_image_block, (unsigned short*) img.data, (img.width * img.height) / 2);
}


void sprite_init(struct sprite* sprite, struct sprite_attr* attr, int width)
{

	sprite->attr = attr;
		//defults to zero
	sprite->frame = sprite->delay =  sprite->counter = sprite->is_moving= sprite->is_flipped=0;
	sprite->width = width;
}

/*
	Sets sprite attribute in vram 
*/
struct sprite_attr* sprite_make_attr(enum sprite_size size, int priority)

{
	if(sprite_attr_index < SPRITE_NUM)
	{
		int h_flip =0,  v_flip =0,  tile_offset =0; //defaults 
		int size_flag, shape_flag;
    	switch (size) {
	        case SPRITE_8_8:   size_flag = 0; shape_flag = 0;  break;
	        case SPRITE_16_16: size_flag = 1; shape_flag = 0;  break;
	        case SPRITE_32_32: size_flag = 2; shape_flag = 0;  break;
	        case SPRITE_64_64: size_flag = 3; shape_flag = 0;  break;
	        case SPRITE_16_8:  size_flag = 0; shape_flag = 1;  break;
	        case SPRITE_32_8:  size_flag = 1; shape_flag = 1;  break;
	        case SPRITE_32_16: size_flag = 2; shape_flag = 1;  break;
	        case SPRITE_64_32: size_flag = 3; shape_flag = 1;  break;
	        case SPRITE_8_16:  size_flag = 0; shape_flag = 2;  break;
	        case SPRITE_8_32:  size_flag = 1; shape_flag = 2;  break;
	        case SPRITE_16_32: size_flag = 2; shape_flag = 2;  break;
	        case SPRITE_32_64: size_flag = 3; shape_flag = 2;  break;
    	}
		sprite_attrs[sprite_attr_index].attr0 =
					 0 //8 bits for y value
				   	|(0 << 8)   //affine
				   	|(0 << 10)	//effect
				   	|(0 << 12) //mosaic
				  	|(COLOR_MODE << 13) //color mode from GBA.h
					|((shape_flag & 0x03) << 14); //mask 2 bits of shape

		sprite_attrs[sprite_attr_index].attr1 =
     					0 //9 bits
						| (0 << 9)           // not affine flag 
                        |   (h_flip << 12)          // horizontal flip flag 
                        |   (v_flip << 13)          // vertical flip flag 
                        |   (size_flag << 14);  // size 

    	sprite_attrs[sprite_attr_index].attr2 =
    				 	tile_offset   // tile index 
                         |   (priority << 10) //priority 
                         |   (0 << 12);          // 16 color palette	
	}
	//return sprite struct with the given sprite attributess
	return  &sprite_attrs[sprite_attr_index++];
}

// set sprite pos
void sprite_set_pos(struct sprite* sprite, int x, int y) {
    // clear lower 8 bits of attr0
    sprite->x = x;
    sprite->y = y;
    sprite->attr->attr0 &= 0xff00;
    // set the new y pos in lower 8 bits by clearing the upper bits 8 of x
    sprite->attr->attr0 |= (y & 0xff);
    // clear lower 9 bits of attr1
    sprite->attr->attr1 &= 0xfe00;
    // set the new x pos in lower 9 bits by clearing the upper bits 7 of x
    sprite->attr->attr1 |= (x & 0x1ff);
}


// update the tile offset of a sprite
void sprite_set_offset(struct sprite* sprite, int offset) {
    /* clear the old offset */
    sprite->attr->attr2 &= 0xfc00;
    /* apply the new one */
    sprite->attr->attr2 |= (offset & 0x03ff);
}

//Move sprites position by dx, dy
void sprite_move_by(struct sprite* sprite, int dx, int dy) {
    //add by deltas
    sprite_set_pos(sprite,  sprite->x + dx,  sprite->y + dy);
}



void sprite_flip(struct sprite* sprite, int h_flip, int v_flip){
	//Attr1 | 0 0 V  H  |0 0 0 0 |0 0 0 0 |0 0 0 0
	// 	H	| 1 1 1  0  |1 1 1 1 |1 1 1 1 |1 1 1 1 = clear: & 0xEFFF set: | 0x1000 
	// 	V	| 1 1 0  1  |1 1 1 1 |1 1 1 1 |1 1 1 1 = clear: & 0xDFFF set: | 0x2000
	if(v_flip)
		sprite->attr->attr1 |=  0x2000 ;//set flags 
	else
		sprite->attr->attr1 &=   0xDFFF;   //clear  flip flags 
	
	if(h_flip)
		sprite->attr->attr1 |=  0x1000 ;//set flags 
	else
		sprite->attr->attr1 &=   0xEFFF;   //clear  flip flags 
}

// clear all sprites data 
void sprite_clear_all() {
    /* clear the index counter */
    sprite_attr_index = 0;
    /* move all sprites offscreen to hide them */
    for(int i = 0; i < SPRITE_NUM; i++) {
        sprite_attrs[i].attr0 = (SCREEN_HEIGHT & 0xff);
        sprite_attrs[i].attr1 = (SCREEN_WIDTH& 0x1ff);
        sprite_attrs[i].attr2 = 0;
    }
}


void sprite_update_all() {
    memcpy_dma16((unsigned short*) sprite_attribute_block, (unsigned short*) sprite_attrs, SPRITE_NUM * 4);
}
#endif






