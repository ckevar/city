#include <allegro.h>
#include "tlgraphics.h"
#include "utils.h"

/*Switches traffic light to col_dest*/
void switchLight(int* tl_matrix, const int i, const int j, const int n_blocks_x,
				const int n_blocks_y, const int col_dest){

	tl_matrix[i*n_blocks_y + j] = col_dest;

}

/*Draws the initial traffic lights and returns a pointer to them*/
void initTrafficLights(int* tl_matrix, const int n_blocks_x, 
						const int n_blocks_y, const int someCoeff,
						const int street_w, const int block_w){
	int i, j;

	for(i = 0; i < n_blocks_x*2; ++i){
		for(j = 0; j < n_blocks_y*2; ++j){

			tl_matrix[i*n_blocks_y + j] = -1;

			/*corners of the considered block*/
			int x_right, y_up, x_left, y_low;

			x_right	=	i*someCoeff + street_w + block_w;
			y_up	=	j*someCoeff + street_w;
			x_left	=	i*someCoeff + street_w;
			y_low	=	j*someCoeff	+ street_w + block_w;

			/*checking if there are traffic lights in the corners of the block*/
			if(getpixel(screen, x_left + BL_BORDER + 1, y_up + BL_BORDER + 1) == TL_COL){
				tl_matrix[i*n_blocks_y + j] = TL_RED;
				circlefill(screen, x_left + BL_BORDER + TL_SIZE/2,
						y_up + BL_BORDER + TL_SIZE/2, LIGHTS_RAY, TL_RED);
			}
			if(getpixel(screen, x_right - BL_BORDER - 1, y_up + BL_BORDER + 1) == TL_COL){
				tl_matrix[i*n_blocks_y + j] = TL_RED;
				circlefill(screen, x_right - BL_BORDER - TL_SIZE/2,
						y_up + BL_BORDER + TL_SIZE/2, LIGHTS_RAY, TL_RED);
			}
			if(getpixel(screen, x_left + BL_BORDER + 1, y_low - BL_BORDER - 1) == TL_COL){
				tl_matrix[i*n_blocks_y + j] = TL_RED;
				circlefill(screen, x_left + BL_BORDER + TL_SIZE/2,
						y_low - BL_BORDER - TL_SIZE/2, LIGHTS_RAY, TL_RED);
			}
			if(getpixel(screen, x_right - BL_BORDER - 1, y_low - BL_BORDER - 1) == TL_COL){
				tl_matrix[i*n_blocks_y + j] = TL_RED;
				circlefill(screen, x_right - BL_BORDER - TL_SIZE/2,
						y_low - BL_BORDER - TL_SIZE/2, LIGHTS_RAY, TL_RED);
			}
		}
	}

	return;
}

/*Draws traffic lights on a block based on the map*/
void drawTLCabins(const int street_w, const int block_w,
						const int i, const int j, const int someCoeff, 
						const int n_blocks_x, const int n_blocks_y){

	/*corners of the considered block*/
	int x_right, y_up, x_left, y_low;

	x_right	=	i*someCoeff + street_w + block_w;
	y_up	=	j*someCoeff + street_w;
	x_left	=	i*someCoeff + street_w;
	y_low	=	j*someCoeff	+ street_w + block_w;

	int unique_right, unique_left, unique_top, unique_bottom;

	unique_right	=	(getpixel(screen, x_right + 1, y_up) == BLOCK_COL) ? 0 : 1;
	unique_left		=	(getpixel(screen, x_left - 1, y_up) == BLOCK_COL) ? 0 : 1;
	unique_top 		=	(getpixel(screen, x_left, y_up - 1) == BLOCK_COL) ? 0 : 1;
	unique_bottom	=	(getpixel(screen, x_left, y_low + 1) == BLOCK_COL) ? 0 : 1;

	/*top right of the block*/
	if(unique_right && unique_top){
		if(!(i == n_blocks_x - 1 && j == 0)){
			rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_right - BL_BORDER, y_up + BL_BORDER, TL_COL);
			rect(screen, x_right - TL_SIZE - BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_right - BL_BORDER, y_up + BL_BORDER, 0);
		}
	}

	/*bottom right of the block*/
	if(unique_right && unique_bottom){
		if(!(i == n_blocks_x - 1 && j == n_blocks_y - 1)){
			rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
					x_right - BL_BORDER, y_low - TL_SIZE - BL_BORDER, TL_COL);
			rect(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
					x_right - BL_BORDER, y_low - TL_SIZE - BL_BORDER, 0);
		}

	}

	/*top left of the block*/
	if(unique_left && unique_top){
		if(i || j){
			rectfill(screen, x_left + BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, TL_COL);
			rect(screen, x_left + BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, 0);
		}
	}

	/*bottom left of the block*/
	if(unique_left && unique_bottom){
		if(!(i == 0 && j == n_blocks_y - 1)){
			rectfill(screen, x_left + BL_BORDER, y_low - BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_low - TL_SIZE - BL_BORDER, TL_COL);
			rect(screen, x_left + BL_BORDER, y_low - BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_low - TL_SIZE - BL_BORDER, 0);
		}
	}

	return;
}