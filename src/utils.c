#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

void initGridMap(const char n_blocks){
	char n_blocks_x, n_blocks_y;
	int i, j;

	int block_w = 40;
	int street_w = 20;
	int someCoeff = block_w + street_w;

	n_blocks_x = W / (2 * n_blocks);
	n_blocks_y = H / (2 * n_blocks);

	for(i = 0; i < n_blocks_x; i++){
		for(j = 0; j < n_blocks_y; j++){
			// rectfill(screen, (i * 20 + 5), (j * 15 + 10), 
			// 				 (i * 15 + 5), (j * 20 + 10), 10);
			// rectfill(screen, (j * 20), (i * 20), 
			// 				 (j * 20 + block_w), 
			// 				 (i * 20 + block_w), 10);
			rectfill(screen, (j * someCoeff), (i * someCoeff), 
							 (j * someCoeff + block_w), 
							 (i * someCoeff + block_w), 10);
		}
	}	
	// street_w = 20(j+1) - (20j + block_w) = 20j + 20 - 20j - block_w
	// street_w = 20 - block_w;
	// street_w + block_w = 20 = someCoeff;
}

