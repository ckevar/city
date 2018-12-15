#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "tlgraphics.h"

/*Draws street lines on the map*/
void drawStreetLines(const int street_w, const int block_w, const int someCoeff,
						const int n_blocks_x, const int n_blocks_y){
	
	line(screen, street_w / 2, street_w / 2,
				n_blocks_x*block_w + n_blocks_x*street_w + street_w / 2, 
				street_w / 2, SL_COL);
	line(screen, street_w / 2, street_w / 2, street_w / 2,
				n_blocks_y*block_w + n_blocks_y*street_w + street_w / 2, SL_COL);
	line(screen, n_blocks_x*block_w + n_blocks_x*street_w + street_w / 2, 
				street_w / 2,
				n_blocks_x*block_w + n_blocks_x*street_w + street_w / 2,
				n_blocks_y*block_w + n_blocks_y*street_w + street_w / 2, SL_COL);
	line(screen, street_w / 2,
				n_blocks_y*block_w + n_blocks_y*street_w + street_w / 2,
	 			n_blocks_x*block_w + n_blocks_x*street_w + street_w / 2, 
	 			n_blocks_y*block_w + n_blocks_y*street_w + street_w / 2, SL_COL);

	int i, j;
	for(i = 0; i < n_blocks_x; i++){
		for(j = 0; j < n_blocks_y; j++){
			/*vertical lines*/
			//if(getpixel(screen, i*someCoeff + street_w / 2, ))
		}
	}
}


/*Initializes a regular grid with specified dimensions*/
void initGridMap(const int street_w, const int block_w){
	char n_blocks_x, n_blocks_y;
	int i, j;

	int someCoeff = block_w + street_w;

	n_blocks_x = (W - street_w) / (block_w + street_w);
	n_blocks_y = (H - street_w) / (block_w + street_w);

	for(i = 0; i < n_blocks_x; i++){
		for(j = 0; j < n_blocks_y; j++){
			rectfill(screen, (i * someCoeff) + street_w, 
							(j * someCoeff) + street_w, 
							(i * someCoeff) + block_w + street_w, 
							(j * someCoeff) + block_w + street_w, BLOCK_COL);
		}
	}	
}

/*Initializes a random grid with specified dimensions*/
void initRandomMap(int *tl_matrix){

	int current_blocks = 0;
	int i, j;

	int someCoeff = BLOCK_W + STREET_W;
	int random_factor;
	int a, b;

	srand (time(NULL));

	for (i = 0; i < N_BLOCKS_X * 2; i++) {
		for (j = 0; j < N_BLOCKS_Y * 2; j++) {
			tl_matrix[j * N_BLOCKS_X * 2 + i] = 0;
		}
	}

	for(i = 0; i < N_BLOCKS_X; i++){
		for(j = 0; j < N_BLOCKS_Y; j++){

			random_factor = rand() % 100;

			if(random_factor >= 50 || (i == N_BLOCKS_X - 1) || (j == N_BLOCKS_Y - 1)) {
				a = 1; b = 1;
			}
			else if(random_factor >= 25){
				a = 2; b = 1;
			}
			else{
				a = 1; b = 2;
			}
			rectfill(screen, (i * someCoeff) + STREET_W, 
							 (j * someCoeff) + STREET_W, 
							 (i * someCoeff) + BLOCK_W + a * STREET_W, 
							 (j * someCoeff) + BLOCK_W + b * STREET_W, BLOCK_COL);


			drawTLCabins(tl_matrix, i, j, someCoeff);
		}
	}

	//initTrafficLights(tl_matrix,N_BLOCKS_X, N_BLOCKS_Y, someCoeff, STREET_W, BLOCK_W);

	//drawStreetLines(street_w, BLOCK_W, someCoeff, n_blocks_x, n_blocks_y);
}


