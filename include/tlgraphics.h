#ifndef TRAFF_LIGHT_H
#define TRAFF_LIGHT_H

/*Draws the traffic lights and returns a pointer to them*/
void initTrafficLights( int* tl_matrix, const int n_blocks_x, 
						const int n_blocks_y, const int someCoeff, 
						const int street_w, const int block_w);

/*Draws traffic lights on a block based on the map*/
void drawTLCabins(int* tl_matrix, const int i, const int j, const int someCoeff);

/*Switches traffic light to col_dest*/
void switchLight(int* tl_matrix, const int i, const int j,
				const int someCoeff, const int col_dest);

#endif