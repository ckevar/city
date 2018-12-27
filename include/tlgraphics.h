#ifndef TRAFF_LIGHT_H
#define TRAFF_LIGHT_H

/*Draws traffic lights on a block based on the map*/
void drawTLCabins(int* tl_matrix, const int i, const int j, const int someCoeff);

/*Switches traffic light to col_dest*/
void switchLight(int* tl_matrix, const int i, const int j,
				const int someCoeff, const int col_dest, const int col_mat);

#endif