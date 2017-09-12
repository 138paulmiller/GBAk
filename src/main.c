#include <gba.h>
#include <interrupt.h>
#include <bg.h>
#include <sprite.h>

#include <img/sprite0.h>
#include <img/bg_img0.h>
#include <img/map_data0.h>
#include <img/map_data1.h>

int main( ) { 
    *display_control = MODE0 | BG0 | BG1 | SPRITE_ENABLE | SPRITE_MAP_1D;

	struct bg bg0 = (struct bg)	{bg_img0_data, bg_img0_palette, bg_img0_width, bg_img0_height};
	struct bg_map map0 = 	(struct bg_map)	{map_data0, map_data0_width, map_data0_height};
	struct bg_map map1 = 	(struct bg_map)	{map_data1, map_data1_width, map_data1_height};
	
	
	struct sprite_img sprite_img0 = (struct sprite_img) {sprite0_data, sprite0_palette, sprite0_width, sprite0_height, 2}; //2 frames
	struct sprite *sprite0;  //points to a sprite in sprites.h sprite_array
	//init bg
	bg_load(bg0, 0);
	bg_map_load(map0, 8);
	bg_set(bg0_control, 0, 8, BG_32x32, 0, BG_NO_WRAP); 
	//bg_map_load(map1, 9);
	//bg_set(bg1_control, 0, 9, BG_32x32, 1, BG_NO_WRAP); 
	

	//init sprites
	sprite_clear_all(); //clear memory
	sprite_load_img(sprite_img0);
	sprite0 = sprite_make(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SPRITE_16_32, 0, 0, 0, 0);


	//main loop
	int scrollx=0;
	int scrolly=0;   
	while (1)
	{
		vblank_wait();
		/* scroll with the arrow keys */
        if (button_pressed(BUTTON_DOWN)) {
            scrolly++;
        }
        if (button_pressed(BUTTON_UP)) {
            scrolly--;

        }
        if (button_pressed(BUTTON_RIGHT)) {
            scrollx++;
        }
        if (button_pressed(BUTTON_LEFT)) {
            scrollx--;
        }

        //update bg
		*bg0_x_scroll = scrollx;
    	*bg0_y_scroll = scrolly;
		//*bg1_x_scroll = scrollx;
    	//*bg1_y_scroll = scrolly;
		//update sprite
		sprite_update_all() ;

		delay(20);	

	} //wait
}
