#include <stdio.h>
#include <allegro.h>
#include "types.h"
#include "vehicle.h"

/*RESOLUTION CONSTANTS*/
#define W 640
#define H 480
/*BITS FOR COLORS*/
#define N_COL 8

void initialize_graphics() 
{
	set_color_depth(N_COL);

	set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, H, 0, 0);
}

int main(int argc, char const *argv[])
{
	allegro_init();

	initialize_graphics();

	while(1){}
	
	allegro_exit();

	return 0;
}
