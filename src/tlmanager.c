#include "tlmanager.h"
#include "utils.h"
#include "tlgraphics.h"

#include <stdio.h>

void tl_init() {
	sleep(4);
}

void tl_manager(void *arg){
	tl_manager_arg *myarg = (tl_manager_arg *) arg;
	int *tl_matrix;
	int i, j;

	// myarg->tl_matrix = tl_matrix; 
	
	/*naive implementation of switch lights system*/
	for(i = 0; i < N_BLOCKS_X*2; i++){
		for(j = 0; j < N_BLOCKS_Y*2; j++){

			if(myarg->tl_matrix[j * N_BLOCKS_X * 2 + i] == TL_GREEN){
				switchLight(myarg->tl_matrix, i, j, SOMECOEFF, TL_YELLOW);
			}
			else if(myarg->tl_matrix[j * N_BLOCKS_X * 2 + i] == TL_YELLOW){
				switchLight(myarg->tl_matrix, i, j, SOMECOEFF, TL_RED);
			}
			else if(myarg->tl_matrix[j * N_BLOCKS_X * 2 + i] == TL_RED){
				switchLight(myarg->tl_matrix, i, j, SOMECOEFF, TL_GREEN);
			}
			
		}
	}
}