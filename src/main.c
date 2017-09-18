#include <time.h>

#include <gba/gba.h>

#include <img/text.h>
#include <img/bg_img0.h>
#include <img/map_data0.h>
#include <img/map_data1.h>


#include <img/sprite0_img.h>


#define BORDER_X 50
#define BORDER_Y 20
#define PRIORITY_FIRST 3
#define PRIORITY_SECOND 2
#define PRIORITY_THIRD 1
#define PRIORITY_LAST 0
int main( ) { 
	clock_t clock_start ; //timer
    *display_control = MODE1 | BG0 | BG1 | SPRITE_ENABLE | SPRITE_MAP_1D;

    //background image and palette for text, tilemap is the string of chars to display 
	struct bg bg_text = (struct bg)	{text_data, text_palette, text_width, text_height}; //load into screen_block 0


	struct bg bg0 = (struct bg)	{bg_img0_data, bg_img0_palette, bg_img0_width, bg_img0_height};
	struct bg_tilemap tilemap0 = 	(struct bg_tilemap)	{map_data0, map_data0_width, map_data0_height};
	
	struct sprite_img sprite_img0 = (struct sprite_img) {sprite0_img_data, sprite0_img_palette, 
														sprite0_img_width, sprite0_img_height};
	struct sprite sprite_player;
	//init bgs
	
	//level bg
	bg_load_image(&bg0, 0);	
	bg_load_palette(bg0.palette);
 	bg_load_tilemap(&tilemap0, 24);
 	//bg1 is using char block 1, screen block 24
	bg_set(bg1_control, 0, 24, BG_32x32, PRIORITY_FIRST, BG_NO_WRAP);  //no wrap only works in affine mode

	
	//init the text bg
	bg_load_image(&bg_text, 1); 
	// bg_load_palette(bg_text.palette);
 	bg_clear_screen_block(25);
	// //tilemap is set by set_tet function!
	bg_set_text("HELLO!", 25, 32, 10, 20);
	bg_set(bg0_control, 1, 25, BG_32x32, PRIORITY_LAST, BG_NO_WRAP);



	//init sprites
	sprite_clear_all(); //clear memory
	sprite_load_img(sprite_img0);
	sprite_init(&sprite_player, sprite_make_attr(SPRITE_16_32, 0), 16, 32, 2); //2 16x32 bit frames
	sprite_player.x = SCREEN_WIDTH/2;
    sprite_player.y = SCREEN_HEIGHT/2;
  	sprite_player.is_moving = 1;
	sprite_update(&sprite_player);

    float delta = 0;
	//main loop
	int bg_offset_x=0;
	int bg_offset_y=0;   
	while (1)
	{
		vblank_wait();
		clock_start = clock();
		sprite_player.is_moving = 0;
		sprite_player.vel_x = sprite_player.vel_y = 0; //by default
		/* scroll with the arrow keys */
        if (button_pressed(BUTTON_UP)) 
        {
        	sprite_flip(&sprite_player, 0, 0);
		    sprite_player.is_moving = 1;
    		sprite_player.vel_y = -1;

        }
        if (button_pressed(BUTTON_DOWN)) 
        {
	        sprite_flip(&sprite_player, 0, 0);
		    sprite_player.is_moving = 1;
    		sprite_player.vel_y = 1;

		}
        //left right directional
        if (button_pressed(BUTTON_RIGHT)) 
        {
			sprite_flip(&sprite_player, 0, 0);
	      	sprite_player.is_moving = 1;
    		sprite_player.vel_x = 1;
	    
        }
        if (button_pressed(BUTTON_LEFT)) 
        {
        	sprite_flip(&sprite_player, 1, 0);
		    sprite_player.is_moving = 1;
	    	sprite_player.vel_x = -1;
			
		}
		//check collisions with tiles
		
		//move border and stop player if near screen edge
		if(sprite_player.x+sprite_player.vel_x < BORDER_X ||
			sprite_player.x+sprite_player.vel_x + sprite_player.width > SCREEN_WIDTH-BORDER_X){
	            //move bg_by velocity and zero out player vel
	            bg_offset_x+=sprite_player.vel_x; 
	            sprite_player.vel_x = 0; 
	    }
	    if(	sprite_player.y + sprite_player.vel_y < BORDER_Y ||
	    	sprite_player.y + sprite_player.vel_y + sprite_player.height > SCREEN_HEIGHT-BORDER_Y){
	            bg_offset_y+=sprite_player.vel_y;
	            sprite_player.vel_y = 0; 
	    }

		//moves player and updates animation frame
		sprite_update(&sprite_player);
		//update bg1 using control1
    	bg_set_scroll(1,  bg_offset_x,  bg_offset_y);
		//update sprites
		sprite_update_all() ;
		delta = clock() - clock_start; 
		if(delta < 50)
			delay(50-delta);	

	} //wait
}
