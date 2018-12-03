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

	/*Initializing a simple random map*/
	int n_blocks_x, n_blocks_y;
	n_blocks_x = W/(2*n_blocks);
	n_blocks_y = H/(2*n_blocks);
	int i,j;
	for(i=0; i<n_blocks_x; i++){
		for(j=0; j<n_blocks_y; j++){
			rectfill(screen,(i*20+5),(j*15+10),(i*15+5),(j*20+10),10);
		}
	}
}

int main(int argc, char const *argv[])
{
	allegro_init();

	n_blocks = 30;	//for now

	initialize_graphics();

	while(1){}
	
	allegro_exit();

	return 0;
}
