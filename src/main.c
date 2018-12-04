#include <stdio.h>
#include <allegro.h>

#include "types.h"
#include "vehicle.h"
#include "utils.h"

void initialize_graphics() 
{
	set_color_depth(N_COL);

	set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, H, 0, 0);

	clear_to_color(screen, STREET_COL);

	/*Initializing a simple random map*/
	initRandomMap(STREET_W);
}

int main(int argc, char const *argv[])
{
	allegro_init();

	//n_blocks = 30;	//for now

	initialize_graphics();

	/**** Testing Cam *****/
	vehicle_t v1;
	initVehicle(&v1);
	getFrame(&v1);
	/**********************/

	while(1){}
	
	allegro_exit();

	return 0;
}
