#include <time.h>

#include <gba/gba.h>


#include <img/bg_img0.h>
#include <img/map_data0.h>
#include <img/map_data1.h>


#include <img/sprite0_img.h>


#define BORDER 50

int main( ) { 
	clock_t clock_start ; 
    *display_control = MODE0 | BG0 | SPRITE_ENABLE | SPRITE_MAP_1D;

	struct bg bg0 = (struct bg)	{bg_img0_data, bg_img0_palette, bg_img0_width, bg_img0_height};
	struct bg_map map0 = 	(struct bg_map)	{map_data0, map_data0_width, map_data0_height};
	struct bg_map map1 = 	(struct bg_map)	{map_data1, map_data1_width, map_data1_height};
	
	
	struct sprite_img sprite_img0 = (struct sprite_img) {sprite0_img_data, sprite0_img_palette, 
														sprite0_img_width, sprite0_img_height};
	struct sprite sprite_player;
	//init bg
	bg_load(bg0, 0);
	bg_map_load(map0, 8);
	bg_set(bg0_control, 0, 8, BG_32x32, 0, BG_NO_WRAP); 
	//bg_map_load(map1, 9);
	//bg_set(bg1_control, 0, 9, BG_32x32, 1, BG_NO_WRAP); 
	

	//init sprites
	sprite_clear_all(); //clear memory
	sprite_load_img(sprite_img0);
	sprite_init(&sprite_player, sprite_make_attr(SPRITE_16_32, 0), 16, 2); //2 16 bit frames
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
		/* scroll with the arrow keys */
        if (button_pressed(BUTTON_DOWN)) {
        }
        if (button_pressed(BUTTON_UP)) {
        }
        //left right directional
        if (button_pressed(BUTTON_RIGHT)) {
			sprite_flip(&sprite_player, 0, 0);
	      	sprite_player.is_moving = 1;
        	if( sprite_player.x > SCREEN_WIDTH-BORDER){	
				bg_offset_x++;
			}
			else{
	    		sprite_player.x ++;
	        }
        }
        if (button_pressed(BUTTON_LEFT)) {
        	sprite_flip(&sprite_player, 1, 0);
		    sprite_player.is_moving = 1;
	        if(sprite_player.x < BORDER){
	            bg_offset_x--;
	        }
	        else{
	    		sprite_player.x --;

			}

		}
		sprite_update(&sprite_player);

		//update bg
    	bg_set_scroll(0,  bg_offset_x,  bg_offset_y);
		//update sprites
		sprite_update_all() ;
		// delta = clock() - clock_start; 
		if(delta < 50)
			delay(50-delta);	

	} //wait
}
