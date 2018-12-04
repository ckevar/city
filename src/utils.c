#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

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
							(j * someCoeff) + block_w + street_w, 2);
		}
	}	
}

/*Initializes a random grid with specified dimensions*/
void initRandomMap(const int street_w){
	char n_blocks_x, n_blocks_y;
	int i, j;

	int someCoeff = BLOCK_W + street_w;
	int random_factor;
	srand (time(NULL));

	n_blocks_x = (W - street_w) / (BLOCK_W + street_w);
	n_blocks_y = (H - street_w) / (BLOCK_W + street_w);

	for(i = 0; i < n_blocks_x; i++){
		for(j = 0; j < n_blocks_y; j++){

			random_factor = rand() % 100;

			if(random_factor >= 50 || (i == n_blocks_x - 1) || (j == n_blocks_y - 1)){
				rectfill(screen, (i * someCoeff) + street_w, 
								(j * someCoeff) + street_w, 
								(i * someCoeff) + BLOCK_W + street_w, 
								(j * someCoeff) + BLOCK_W + street_w, 2);
			}
			else if(random_factor >= 25){
				rectfill(screen, (i * someCoeff) + street_w, 
								(j * someCoeff) + street_w, 
								(i * someCoeff) + BLOCK_W + 2*street_w, 
								(j * someCoeff) + BLOCK_W + street_w, 2);
			}
			else{
				rectfill(screen, (i * someCoeff) + street_w, 
								(j * someCoeff) + street_w, 
								(i * someCoeff) + BLOCK_W + street_w, 
								(j * someCoeff) + BLOCK_W + 2*street_w, 2);
			}
			/*
			else{
				rectfill(screen, (i * someCoeff) + street_w, 
								(j * someCoeff) + street_w, 
								(i * someCoeff) + BLOCK_W + 2*street_w, 
								(j * someCoeff) + BLOCK_W + 2*street_w, 2);
			}
			*/
		}
	}
}

