#include <allegro.h>
#include "tlgraphics.h"
#include "utils.h"

#include <stdio.h>

/*Switches traffic light to col_dest*/
void switchLight(int* tl_matrix, const int i, const int j,
				 const int someCoeff, const int col_dest, const int col_mat){

	if(tl_matrix[j*N_BLOCKS_X*2 + i] == -1)
		return;

	int block_x, block_y;

	block_x = i / 2;
	block_y = j / 2;

	/*corners of the considered block*/
	int x_right, y_up, x_left, y_low;

	x_right	=	block_x*someCoeff + STREET_W + BLOCK_W;
	y_up	=	block_y*someCoeff + STREET_W;
	x_left	=	block_x*someCoeff + STREET_W;
	y_low	=	block_y*someCoeff + STREET_W + BLOCK_W;

	int x_tl, y_tl;

	if(i%2){	//right side of the block
		x_tl = x_right - BL_BORDER - TL_SIZE/2;
	}
	else {		//left side of the block
		x_tl = x_left + BL_BORDER + TL_SIZE/2;
	}

	if(j%2) {	//bottom of the block
		y_tl = y_low - BL_BORDER - TL_SIZE/2;
	} 
	else{		//top of the block
		y_tl = y_up + BL_BORDER + TL_SIZE/2;
	}

	tl_matrix[j*N_BLOCKS_X*2 + i] = col_mat;
	circlefill(screen, x_tl, y_tl, LIGHTS_RAY, col_dest);


	if(i%2 && j%2){			//bottom right of the screen => half a circle top cabinet
		rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
			x_right - BL_BORDER, y_tl, TL_COL);
		rect(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
			x_right - BL_BORDER, y_low - TL_SIZE - BL_BORDER, 0);
	}

	else if(i%2 && !(j%2)){	//top right of the block => half a circle left cabinet
		rectfill(screen, x_tl, y_up + TL_SIZE + BL_BORDER,
			x_right - BL_BORDER, y_up + BL_BORDER, TL_COL);
		rect(screen, x_right - TL_SIZE - BL_BORDER, y_up + TL_SIZE + BL_BORDER,
			x_right - BL_BORDER, y_up + BL_BORDER, 0);
	}

	else if(!(i%2) && j%2){	//bottom left of the block => half a circle right cabinet
		rectfill(screen, x_left + BL_BORDER, y_low - BL_BORDER,
			x_tl, y_low - TL_SIZE - BL_BORDER, TL_COL);
		rect(screen, x_left + BL_BORDER, y_low - BL_BORDER,
			x_left + TL_SIZE + BL_BORDER, y_low - TL_SIZE - BL_BORDER, 0);
	}
	else{					//top left of the block => half a circle bottom cabinet
		rectfill(screen, x_left + BL_BORDER, y_tl,
			x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, TL_COL);
		rect(screen, x_left + BL_BORDER, y_up + TL_SIZE + BL_BORDER,
			x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, 0);
	}
}

/*Draws traffic lights on a block based on the map*/
void drawTLCabins(int* tl_matrix, const int i, const int j, const int someCoeff){

	/*corners of the considered block*/
	int x_right, y_up, x_left, y_low;

	x_right	=	i*someCoeff + STREET_W + BLOCK_W;
	y_up	=	j*someCoeff + STREET_W;
	x_left	=	i*someCoeff + STREET_W;
	y_low	=	j*someCoeff	+ STREET_W + BLOCK_W;

	int unique_right, unique_left, unique_top, unique_bottom;

	unique_right	=	(getpixel(screen, x_right + 1, y_up) == BLOCK_COL) ? 0 : 1;
	unique_left		=	(getpixel(screen, x_left - 1, y_up) == BLOCK_COL) ? 0 : 1;
	unique_top		=	(getpixel(screen, x_left, y_up - 1) == BLOCK_COL) ? 0 : 1;
	unique_bottom	=	(getpixel(screen, x_left, y_low + 1) == BLOCK_COL) ? 0 : 1;

	int top_right_corner, bottom_right_corner, top_left_corner, bottom_left_corner;

	top_right_corner	=	((getpixel(screen, x_right + STREET_W/2, y_up - STREET_W - 5) == BLOCK_COL)
								&& (getpixel(screen, x_right + STREET_W + 5, y_up - STREET_W/2) == BLOCK_COL)) ? 0 : 1;

	bottom_right_corner	=	((getpixel(screen, x_right + STREET_W/2, y_low + STREET_W + 5) == BLOCK_COL)
								&& (getpixel(screen, x_right + STREET_W + 5, y_low + STREET_W/2) == BLOCK_COL)) ? 0 : 1;

	top_left_corner		=	((getpixel(screen, x_left - STREET_W/2, y_up - STREET_W - 5) == BLOCK_COL)
								&& (getpixel(screen, x_left - STREET_W - 5, y_up - STREET_W/2) == BLOCK_COL)) ? 0 : 1;

	bottom_left_corner	=	((getpixel(screen, x_left - STREET_W/2, y_low + STREET_W + 5) == BLOCK_COL)
								&& (getpixel(screen, x_left - STREET_W - 5, y_low + STREET_W/2) == BLOCK_COL)) ? 0 : 1;

	int t_cross_tleft, t_cross_tright, t_cross_bleft, t_cross_bright;

	t_cross_tleft	=	(!unique_top && unique_left 
							&& (getpixel(screen, x_left - STREET_W - 5, y_up - STREET_W/2) == STREET_COL)
							&& (getpixel(screen, x_left - STREET_W/2, y_up - STREET_W - 5) == STREET_COL)) ? 1 : 0;
	t_cross_tright	=	(!unique_right && unique_top 
							&& (getpixel(screen, x_right + STREET_W + 5, y_up - STREET_W/2) == STREET_COL)
							&& (getpixel(screen, x_right + STREET_W/2, y_up - STREET_W - 5) == STREET_COL)) ? 1 : 0;
	t_cross_bleft	=	(!unique_left && unique_bottom
							&& (getpixel(screen, x_left - STREET_W - 5, y_low + STREET_W/2) == STREET_COL)
							&& (getpixel(screen, x_left - STREET_W/2, y_low + STREET_W + 5) == STREET_COL)) ? 1 : 0;
	t_cross_bright	=	(!unique_bottom && unique_right
							&& (getpixel(screen, x_right + STREET_W + 5, y_low + STREET_W/2) == STREET_COL)
							&& (getpixel(screen, x_right + STREET_W/2, y_low + STREET_W + 5) == STREET_COL)) ? 1 : 0;

	int top_right_needed, bottom_right_needed, top_left_needed, bottom_left_needed;

	top_right_needed	=	(top_right_corner && unique_top && unique_right) || t_cross_tright;
	bottom_right_needed	=	(bottom_right_corner && unique_bottom && unique_right) || t_cross_bright;
	top_left_needed		=	(top_left_corner && unique_top && unique_left) || t_cross_tleft;
	bottom_left_needed	=	(bottom_left_corner && unique_bottom && unique_left) || t_cross_bleft;

	printf("col: %d, row: %d: t_cross_tleft: %d, t_cross_tright: %d, t_cross_bleft: %d, t_cross_bright: %d\n", 
		i, j, t_cross_tleft, t_cross_tright, t_cross_bleft, t_cross_bright);

	/*top right of the block*/
	if(top_right_needed){
		if(!(i == N_BLOCKS_X - 1 && j == 0)){
			rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_right - BL_BORDER, y_up + BL_BORDER, TL_COL);
			tl_matrix[j*2*N_BLOCKS_X*2 + i*2 + 1] = TL_RED*2;

			/*half a circle on left of the cabinet*/
			circlefill(screen, x_right - BL_BORDER - TL_SIZE/2,
				y_up + BL_BORDER + TL_SIZE/2, LIGHTS_RAY, TL_RED);
			rectfill(screen, x_right - TL_SIZE/2 - BL_BORDER, y_up + BL_BORDER,
				x_right - BL_BORDER, y_up + TL_SIZE + BL_BORDER, TL_COL);
			rect(screen, x_right - TL_SIZE - BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_right - BL_BORDER, y_up + BL_BORDER, 0);
		}
	}
	else{tl_matrix[j*2*N_BLOCKS_X*2+ i*2 + 1] = -1;}

	/*bottom right of the block*/
	if(bottom_right_needed){
		if(!(i == N_BLOCKS_X - 1 && j == N_BLOCKS_Y - 1)){
			rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
					x_right - BL_BORDER, y_low - TL_SIZE - BL_BORDER, TL_COL);
			tl_matrix[(j*2+1)*N_BLOCKS_X*2 + i*2 + 1] = TL_YELLOW;

			/*half a circle on top of the cabinet*/
			circlefill(screen, x_right - BL_BORDER - TL_SIZE/2,
				y_low - BL_BORDER - TL_SIZE/2, LIGHTS_RAY, TL_YELLOW);
			rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
				x_right - BL_BORDER, y_low - TL_SIZE/2 - BL_BORDER, TL_COL);
			rect(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
					x_right - BL_BORDER, y_low - TL_SIZE - BL_BORDER, 0);
		}

	}
	else{tl_matrix[(j*2+1)*N_BLOCKS_X*2 + i*2 + 1] = -1;}

	/*top left of the block*/
	if(top_left_needed){
		if(i || j){
			rectfill(screen, x_left + BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, TL_COL);
			tl_matrix[j*2*N_BLOCKS_X*2 + i*2] = TL_YELLOW;

			/*half a circle on the bottom of the cabinet*/
			circlefill(screen, x_left + BL_BORDER + TL_SIZE/2,
				y_up + BL_BORDER + TL_SIZE/2, LIGHTS_RAY, TL_YELLOW);
			rectfill(screen, x_left + BL_BORDER, y_up + TL_SIZE/2 + BL_BORDER,
				x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, TL_COL);
			rect(screen, x_left + BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, 0);
		}
	}
	else{tl_matrix[j*2*N_BLOCKS_X*2 + i*2] = -1;}

	/*bottom left of the block*/
	if(bottom_left_needed){
		if(!(i == 0 && j == N_BLOCKS_Y - 1)){
			rectfill(screen, x_left + BL_BORDER, y_low - BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_low - TL_SIZE - BL_BORDER, TL_COL);
			tl_matrix[(j*2+1)*N_BLOCKS_X*2 + i*2] = TL_RED*2;

			/*half a circle on the right of the cabinet*/
			circlefill(screen, x_left + BL_BORDER + TL_SIZE/2,
				y_low - BL_BORDER - TL_SIZE/2, LIGHTS_RAY, TL_RED);
			rectfill(screen, x_left + BL_BORDER, y_low - BL_BORDER,
				x_left + TL_SIZE/2 + BL_BORDER, y_low - TL_SIZE - BL_BORDER, TL_COL);
			rect(screen, x_left + BL_BORDER, y_low - BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_low - TL_SIZE - BL_BORDER, 0);
		}
	}
	else{tl_matrix[(j*2+1)*N_BLOCKS_X*2 + i*2] = -1;}

	return;
}