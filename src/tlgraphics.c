#include <allegro.h>
#include "tlgraphics.h"
#include "utils.h"

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

/*Draws the initial traffic lights and returns a pointer to them*/
void initTrafficLights(int* tl_matrix, const int n_blocks_x, 
						const int n_blocks_y, const int someCoeff,
						const int street_w, const int block_w){
	int i, j;

	for(i = 0; i < n_blocks_x*2; i++){
		for(j = 0; j < n_blocks_y*2; j++){

			tl_matrix[i*n_blocks_y*2 + j] = -1;

			/*corners of the considered block*/
			int x_right, y_up, x_left, y_low;

			x_right	=	(i/2)*someCoeff + street_w + block_w;
			y_up	=	(j/2)*someCoeff + street_w;
			x_left	=	(i/2)*someCoeff + street_w;
			y_low	=	(j/2)*someCoeff	+ street_w + block_w;

			int color;
			color = TL_RED;

			/*checking if there are traffic lights in the corners of the block*/
			if(!i%2 && !j%2){		//top left traffic light
				if(getpixel(screen, x_left + BL_BORDER + 1, y_up + BL_BORDER + 1) == TL_COL){
					tl_matrix[i*n_blocks_y*2 + j] = color;
					circlefill(screen, x_left + BL_BORDER + TL_SIZE/2,
							y_up + BL_BORDER + TL_SIZE/2, LIGHTS_RAY, color);
				}
			}
			else if(i%2 && !j%2){	//top right traffic light
				if(getpixel(screen, x_right - BL_BORDER - 1, y_up + BL_BORDER + 1) == TL_COL){
					color = TL_GREEN;
					tl_matrix[i*n_blocks_y*2 + j] = color;
					circlefill(screen, x_right - BL_BORDER - TL_SIZE/2,
							y_up + BL_BORDER + TL_SIZE/2, LIGHTS_RAY, color);
				}
			}
			else if(!i%2 && j%2){	//bottom left traffic light
				if(getpixel(screen, x_left + BL_BORDER + 1, y_low - BL_BORDER - 1) == TL_COL){
					tl_matrix[i*n_blocks_y*2 + j] = color;
					circlefill(screen, x_left + BL_BORDER + TL_SIZE/2,
							y_low - BL_BORDER - TL_SIZE/2, LIGHTS_RAY, color);
				}
			}
			else{					//bottom right traffic light
				if(getpixel(screen, x_right - BL_BORDER - 1, y_low - BL_BORDER - 1) == TL_COL){
					color = TL_GREEN;
					tl_matrix[i*n_blocks_y*2 + j] = color;
					circlefill(screen, x_right - BL_BORDER - TL_SIZE/2,
							y_low - BL_BORDER - TL_SIZE/2, LIGHTS_RAY, color);
				}
			}
		}
	}

	return;
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
	unique_top 		=	(getpixel(screen, x_left, y_up - 1) == BLOCK_COL) ? 0 : 1;
	unique_bottom	=	(getpixel(screen, x_left, y_low + 1) == BLOCK_COL) ? 0 : 1;

	/*top right of the block*/
	if(unique_right && unique_top){
		if(!(i == N_BLOCKS_X - 1 && j == 0)){
			rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_right - BL_BORDER, y_up + BL_BORDER, TL_COL);
			tl_matrix[j*2*N_BLOCKS_X*2 + i*2 + 1] = TL_RED;

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
	if(unique_right && unique_bottom){
		if(!(i == N_BLOCKS_X - 1 && j == N_BLOCKS_Y - 1)){
			rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
					x_right - BL_BORDER, y_low - TL_SIZE - BL_BORDER, TL_COL);
			tl_matrix[(j*2+1)*N_BLOCKS_X*2 + i*2 + 1] = TL_GREEN;

			/*half a circle on top of the cabinet*/
			circlefill(screen, x_right - BL_BORDER - TL_SIZE/2,
				y_low - BL_BORDER - TL_SIZE/2, LIGHTS_RAY, TL_GREEN);
			rectfill(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
				x_right - BL_BORDER, y_low - TL_SIZE/2 - BL_BORDER, TL_COL);
			rect(screen, x_right - TL_SIZE - BL_BORDER, y_low - BL_BORDER,
					x_right - BL_BORDER, y_low - TL_SIZE - BL_BORDER, 0);
		}

	}
	else{tl_matrix[(j*2+1)*N_BLOCKS_X*2 + i*2 + 1] = -1;}

	/*top left of the block*/
	if(unique_left && unique_top){
		if(i || j){
			rectfill(screen, x_left + BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, TL_COL);
			tl_matrix[j*2*N_BLOCKS_X*2 + i*2] = TL_RED;

			/*half a circle on the bottom of the cabinet*/
			circlefill(screen, x_left + BL_BORDER + TL_SIZE/2,
				y_up + BL_BORDER + TL_SIZE/2, LIGHTS_RAY, TL_RED);
			rectfill(screen, x_left + BL_BORDER, y_up + TL_SIZE/2 + BL_BORDER,
				x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, TL_COL);
			rect(screen, x_left + BL_BORDER, y_up + TL_SIZE + BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_up + BL_BORDER, 0);
		}
	}
	else{tl_matrix[j*2*N_BLOCKS_X*2 + i*2] = -1;}

	/*bottom left of the block*/
	if(unique_left && unique_bottom){
		if(!(i == 0 && j == N_BLOCKS_Y - 1)){
			rectfill(screen, x_left + BL_BORDER, y_low - BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_low - TL_SIZE - BL_BORDER, TL_COL);
			tl_matrix[(j*2+1)*N_BLOCKS_X*2 + i*2] = TL_GREEN;

			/*half a circle on the right of the cabinet*/
			circlefill(screen, x_left + BL_BORDER + TL_SIZE/2,
				y_low - BL_BORDER - TL_SIZE/2, LIGHTS_RAY, TL_GREEN);
			rectfill(screen, x_left + BL_BORDER, y_low - BL_BORDER,
				x_left + TL_SIZE/2 + BL_BORDER, y_low - TL_SIZE - BL_BORDER, TL_COL);
			rect(screen, x_left + BL_BORDER, y_low - BL_BORDER,
					x_left + TL_SIZE + BL_BORDER, y_low - TL_SIZE - BL_BORDER, 0);
		}
	}
	else{tl_matrix[(j*2+1)*N_BLOCKS_X*2 + i*2] = -1;}

	return;
}