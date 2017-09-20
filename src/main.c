#define DEBUG 1

#include <time.h>
#include <stdio.h>
#include <gba/gba.h>

#include <img/text.h>
#include <img/bg_img0.h>
#include <img/map0.h>
#include <img/sprite0_img.h>

#define BORDER_X 50
#define BORDER_Y 20
#define PRIORITY_FIRST 3
#define PRIORITY_SECOND 2
#define PRIORITY_THIRD 1
#define PRIORITY_LAST 0

/*
Create Sprite Collision and Factory system to load only sprites near the screen into the ATTR array
and to check collisions
For each sprite in level, if outside screen ignore, else if near screen remove from factory into attr array
For each sprite in attr, check collision with all other sprites. When dead remove

//Create a pointer to last removed sprite, if its valid load new sprite into that spot, else push to next attr_index

*/

int main( ) { 
	clock_t clock_start ; //timer
    *display_control = MODE0 | BG0 | BG1 | SPRITE_ENABLE | SPRITE_MAP_1D;

    //background image and palette for text, tilemap is the string of chars to display 
	struct bg bg_text = (struct bg)	{text_data, text_palette, text_width, text_height}; //load into screen_block 0


	struct bg bg0 = (struct bg)	{bg_img0_data, bg_img0_palette, bg_img0_width, bg_img0_height};
	struct bg_tilemap tilemap0 = 	(struct bg_tilemap)	{map0, map0_width, map0_height};
	
	struct sprite_img sprite_img0 = (struct sprite_img) {sprite0_img_data, sprite0_img_palette, 
														sprite0_img_width, sprite0_img_height};
	struct sprite player;
	//init bgs
	
	//level bg
	bg_load_image(&bg0, 0);	
	bg_load_palette(bg0.palette);
 	bg_load_tilemap(&tilemap0, 24);
 	//bg1 is using char block 1, screen block 24
	bg_set(bg0_control, 0, 24, BG_32x32, PRIORITY_FIRST, BG_NO_WRAP);  //no wrap only works in affine mode

	//concat all bg and text palettes?

	//init the text bg
	bg_load_image(&bg_text, 1); 
	// bg_load_palette(bg_text.palette); 
 	bg_clear_screen_block(25);
	// //tilemap is set by set_tet function!
	bg_set(bg1_control, 1, 25, BG_32x32, PRIORITY_LAST, BG_NO_WRAP);


	//init sprites
	sprite_clear_all(); //clear memory
	sprite_load_img(sprite_img0);
	sprite_init(&player, sprite_make_attr(SPRITE_16_32, 0), 16, 32, 2); //2 16x32 bit frames
	player.x = (SCREEN_WIDTH/2 )<<8;
    player.y = (SCREEN_HEIGHT/2 ) <<8;
	sprite_update(&player);

    float delta = 0;
    float jump_height = 1.5;
    int gravity = 980;
    int is_jumping = 0;
	int bg_offset_x=0;
	int bg_offset_y=0;   
	int player_next_x, player_next_y;
	char msg[1000] = {0};


	while (1)
	{

		vblank_wait();

		clock_start = clock();
		player.is_moving = 0;
		player.vel_x = 0;
		if(player.is_falling)
		{		
			sprintf(msg,"FALLING %d", player.vel_y);
			if(player.vel_y < gravity)
				player.vel_y += 128;
			else
				player.vel_y -= 128;
		}
		//Handle Inputs        	
		if (button_pressed(BUTTON_A)) 
        {

        	//hold A for higher jump
        	if(is_jumping)
        	{
        		//peak jump before falling
        		if(jump_height > 1)
        		{
        			sprintf(msg,"HOLD %d", player.vel_y);

        			jump_height -= 0.05;
        			player.vel_y = -gravity*jump_height;
        		}
        		else
        		{
	            	player.is_falling = 1;
        		}

        	}
        	//increase velocity for a few iterations of A down, but cap at 2 
			else if(!player.is_falling)
            	//wait until player hits floor before jumping again
        	{
            	player.vel_y = -gravity*jump_height;
        		is_jumping = 1; //wait ten frames to jump again
    		}
    	}
        else
        {
        	//once release allow jump and reset jump height
        	player.is_falling = 1;
        	is_jumping=0; 
        	jump_height = 1.5;
        }
        //left right directional
        if (button_pressed(BUTTON_RIGHT)) 
        {
			sprite_flip(&player, 0, 0);
			player.is_moving = 1;
    		player.vel_x = 256;	    
        }
        if (button_pressed(BUTTON_LEFT)) 
        {
        	sprite_flip(&player, 1, 0);
		    player.is_moving = 1;
		    player.vel_x = -256;
			
		}

		//check potential move with the ground!
		//get bottom middle of player sprite
		player_next_x = (player.x+player.vel_x) >> 8; 
		player_next_y = (player.y+player.vel_y) >> 8; 
		//check collisions with tiles
		//check feet
		unsigned char tile =  bg_get_tile(&tilemap0, player_next_x+player.width/2, player_next_y+player.height, //error in sprite border 
										 	bg_offset_x, bg_offset_y);
		

		if(tile > 0x0000) //if hit floor
		{
			//move back
			player_next_y = (player.y-player.vel_y) >> 8; 
			player.vel_y = 0; //by default
			player.is_falling = 0;
		}
		else
		{
			player.is_falling = 1;
		}
		//move border and stop player if near screen edge
		if(player_next_x < BORDER_X ||
			player_next_x + player.width > SCREEN_WIDTH-BORDER_X){
	            //move bg_by velocity and zero out player vel
	            if(player.vel_x > 0)
	            	bg_offset_x++;
	        	else
					bg_offset_x--;
	            player.vel_x = 0; 
	    }
	    if(	player_next_y < BORDER_Y ||
	    	player_next_y + player.height > SCREEN_HEIGHT-BORDER_Y){
	            if(player.vel_y > 0)
	            	bg_offset_y++;
	        	else
					bg_offset_y--;
				player.vel_y = 0;
	    }

		sprite_update(&player);
		//update sprites
		sprite_update_all() ;
	
		//update bg1 using control1
    	bg_set_scroll(0,  bg_offset_x,  bg_offset_y);
		//moves player and updates animation frame
		delta = clock() - clock_start; 
		#ifdef DEBUG
		bg_set_text(msg, 25, 32, 0, 0); //update tilemap
		#endif //DEBUG
		delay(400);	

	} //wait
}
