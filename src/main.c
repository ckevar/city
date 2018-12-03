#include <stdio.h>
#include <allegro.h>
#include "types.h"
#include "vehicle.h"

/*RESOLUTION CONSTANTS*/
#define W 800
#define H 600
/*BITS FOR COLORS*/
#define N_COL 8
/*NUMBER OF BLOCKS*/
char n_blocks;

void initialize_graphics() 
{
	set_color_depth(N_COL);

	set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, H, 0, 0);

	clear_to_color(screen, 8);		//grey background

	int i;
	for(i=0; i<n_blocks; i++){
		rectfill(screen,(W+i+10),(H+i+20)/H,(W+i+20)/W,(H+i+10)/10,10);
	}
}

int main(int argc, char const *argv[])
{
	allegro_init();

	n_blocks = 10;	//for now

	initialize_graphics();

	while(1){}
	
	allegro_exit();

	return 0;
}
