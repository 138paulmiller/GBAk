#include <time.h>

#include <gba.h>
#include <interrupt.h>
#include <bg.h>
#include <sprite.h>

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
	
	
	struct sprite_img sprite_img0 = (struct sprite_img) {sprite0_img_data, sprite0_img_palette, sprite0_img_width, sprite0_img_height, 2}; //2 frames
	struct sprite sprite0;
	//init bg
	bg_load(bg0, 0);
	bg_map_load(map0, 8);
	bg_set(bg0_control, 0, 8, BG_32x32, 0, BG_NO_WRAP); 
	//bg_map_load(map1, 9);
	//bg_set(bg1_control, 0, 9, BG_32x32, 1, BG_NO_WRAP); 
	

	//init sprites
	sprite_clear_all(); //clear memory

	sprite_load_img(sprite_img0);
	sprite_init(&sprite0, sprite_make_attr(SPRITE_16_32, 0), 16);
	sprite_set_pos(&sprite0, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    float delta = 0;
	float vel_x=0, acc_x=0.65, friction_x = 0.003;
	//main loop
	int scrollx=0;
	int scrolly=0;   
	while (1)
	{
		clock_start = clock();
		vblank_wait();
		/* scroll with the arrow keys */
        if (button_pressed(BUTTON_DOWN)) {
        }
        if (button_pressed(BUTTON_UP)) {
        }
        if (button_pressed(BUTTON_RIGHT)) {
        	if( sprite0.x > SCREEN_WIDTH-BORDER){	
				scrollx++;
			}
			else{
	    		sprite_flip(&sprite0, 0, 0);
	    		//clamp to 0,1
				if(vel_x > 1) vel_x = 1;
				else if(vel_x < 0) vel_x = 0;
				else vel_x+=acc_x;
	        	sprite_move_by(&sprite0,vel_x, 0);
        	}
        }
        if (button_pressed(BUTTON_LEFT)) {
        	//update bg
	        if(sprite0.x < BORDER){
	            scrollx--;
	        }
	        else{
	    		sprite_flip(&sprite0, 1, 0);
	    		//clamp to -1,0
	    		if(vel_x < -1) vel_x = -1;
	    		else if(vel_x > 0) vel_x = 0;
	    		else vel_x-=acc_x;

	        	sprite_move_by(&sprite0,vel_x, 0);
			}
		}
		else
		{
			vel_x+= -vel_x*friction_x;
		}

      


		//update bg
		*bg0_x_scroll = scrollx;
    	*bg0_y_scroll = scrolly;
		//update sprites
		sprite_update_all() ;
		// delta = clock() - clock_start; 
		if(delta < 10)
			delay(10-delta);	

	} //wait
}
