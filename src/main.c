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
    *display_control = MODE1 | BG0 | BG1 | SPRITE_ENABLE | SPRITE_MAP_1D;

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
	bg_set(bg1_control, 0, 24, BG_32x32, PRIORITY_FIRST, BG_NO_WRAP);  //no wrap only works in affine mode

	//concat all bg and text palettes?

	//init the text bg
	bg_load_image(&bg_text, 1); 
	// bg_load_palette(bg_text.palette); 
 	bg_clear_screen_block(25);
	// //tilemap is set by set_tet function!
	bg_set(bg0_control, 1, 25, BG_32x32, PRIORITY_LAST, BG_NO_WRAP);


	//init sprites
	sprite_clear_all(); //clear memory
	sprite_load_img(sprite_img0);
	sprite_init(&player, sprite_make_attr(SPRITE_16_32, 0), 16, 32, 2); //2 16x32 bit frames
	player.x = (SCREEN_WIDTH/2 )<<8;
    player.y = (SCREEN_HEIGHT/2 ) <<8;


    sprite_set_pos(&player, player.x>>8,player.y>>8);
	//sprite_update(&player);

    float delta = 0;
    //main loop
	int bg_offset_x=0;
	int bg_offset_y=0;   
	int player_next_x, player_next_y;
	char msg[1000] = {0};

	while (1)
	{

		vblank_wait();
		//debug

		clock_start = clock();
        player.is_moving = 0;
		player.vel_x = 0;
		player.vel_y = 0;
	
		if (button_pressed(BUTTON_A)) 
        {
            //action!
        }
        //left right directional
        if (button_pressed(BUTTON_RIGHT)) 
        {
			sprite_flip(&player, 0, 0);
        	player.is_moving = 1;
    		player.vel_x = 100;	    
        }
        if (button_pressed(BUTTON_LEFT)) 
        {
        	sprite_flip(&player, 1, 0);
        	player.is_moving = 1;
		    player.vel_x = -100;
			
		}
		if (button_pressed(BUTTON_DOWN)) 
        {
        	player.is_moving = 1;
			player.vel_y = 100;	    
        }
        if (button_pressed(BUTTON_UP)) 
        {
        	player.is_moving = 1;
		    player.vel_y = -100;
			
		}
		//check potential move with the ground!
		//get bottom middle of player sprite
		// player_next_x = (player.x+player.vel_x) >> 8; 
		// player_next_y = (player.y+player.vel_y) >> 8; 
		//check collisions with tiles
		unsigned char tile =  bg_get_tile(&tilemap0, player_next_x, player_next_y, bg_offset_x, bg_offset_y);
//		sprintf(msg,"%d P:(%d,%d) O:(%d,%d)      ", tile, player_next_x, player_next_y, bg_offset_x, bg_offset_y);
		player.x+=player.vel_x;
		player.y+=player.vel_y;
		int x, y;
		sprite_get_pos(&player, &x, &y);

		sprintf(msg,"(%d,%d)(%d, %d)    ", player.x>>8, player.y>>8,	x,y);

		bg_set_text(msg, 25, 32, 0, 0); //update tilemap

		//if(tile > 0x0000) //if a collision tile
		//{
		//	player.vel_y = 0; //by default
		//	player.vel_x = 0;
		//}
		//else
		// {
		// 	//move border and stop player if near screen edge
		// 	if(player_next_x < BORDER_X ||
		// 		player_next_x + player.width > SCREEN_WIDTH-BORDER_X){
		//             //move bg_by velocity and zero out player vel
		//             if(player.vel_x > 0)
		//             	bg_offset_x++;
		//         	else
		// 				bg_offset_x--;
		//             player.vel_x = 0; 
		//     }
		//     if(	player_next_y < BORDER_Y ||
		//     	player_next_y + player.height > SCREEN_HEIGHT-BORDER_Y){
		//             if(player.vel_y > 0)
		//             	bg_offset_y++;
		//         	else
		// 				bg_offset_y--;
		        	
		//             player.vel_y = 0; 
		//     }
		// }

		//update bg1 using control1
    	bg_set_scroll(1,  bg_offset_x,  bg_offset_y);
		//moves player and updates animation frame

		sprite_update(&player);
		//update sprites
		sprite_update_all() ;
		delta = clock() - clock_start; 
		if(delta < 200)
			delay(200-delta);	

	} //wait
}
