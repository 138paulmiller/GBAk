#include <gba.h>
#include <interrupt.h>
#include <bg.h>
#include <img/bg_img0.h>
#include <img/map_data0.h>
#include <img/map_data1.h>

int main( ) { 
    *display_control = MODE0 | BG0 | BG1;

	struct bg bg0 = (struct bg)	{bg_img0_data, bg_img0_palette, bg_img0_width, bg_img0_height};
	struct map map0 = 	(struct map)	{map_data0, map_data0_width, map_data0_height};
	struct map map1 = 	(struct map)	{map_data1, map_data1_width, map_data1_height};
	load_bg(bg0, 0);
	load_map(map0, 8);
	set_bg(bg0_control, 0, 8, BG_32x32, 0, BG_NO_WRAP); 
	load_map(map1, 9);
	set_bg(bg1_control, 0, 9, BG_32x32, 1, BG_NO_WRAP); 
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
		*bg0_x_scroll = scrollx;
    	*bg0_y_scroll = scrolly;
		//*bg1_x_scroll = scrollx;
    	//*bg1_y_scroll = scrolly;
		delay(20);	

	} //wait
}
