#ifndef UTILS_H
#define UTILS_H

/*RESOLUTION CONSTANTS*/
#define W	1024
#define H	720

/*BITS FOR COLORS*/
#define N_COL	8

/* MAXIMUN KNOWN ENCODED COLORS */
#define MAX_KNOWN_ENCODED_COLORS 15

/*COLORS USED FOR THE MAP*/
#define STREET_COL	15
#define BLOCK_COL	2

/*STANDARD DIMENSIONS OF THE STREETS AND BLOCKS*/
#define BLOCK_W 	100
#define STREET_W 	40

/*NUMBER OF BLOCKS PER AXIS*/
#define N_BLOCKS_X
#define N_BLOCKS_Y

/*COLORS AND SIZE FOR TRAFFIC LIGHTS*/
#define TL_COL		14
#define TL_SIZE		16
#define BL_BORDER	3
#define LIGHTS_RAY	5

/*COLORS FOR TRAFFIC LIGHTS*/
#define TL_GREEN	10
#define TL_YELLOW	42
#define TL_RED		12

/*COLOR STREET LINES*/
#define SL_COL		0

/*Inits grid map*/
void initGridMap(const int street_w, const int block_w);

/*Inits a random map*/
void initRandomMap(int* tl_matrix, const int street_w);

#endif