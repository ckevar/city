#include <allegro.h>

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
#define STREET_COL	makecol(242,242,242)
#define BLOCK_COL	makecol(0,121,107)

/*STANDARD DIMENSIONS OF THE STREETS AND BLOCKS*/
#define BLOCK_W 	100
#define STREET_W 	45

/*NUMBER OF BLOCKS PER AXIS*/
#define N_BLOCKS_X	(W - STREET_W) / (BLOCK_W + STREET_W)
#define N_BLOCKS_Y	(H - STREET_W) / (BLOCK_W + STREET_W)
#define SOMECOEFF	BLOCK_W + STREET_W

/*COLORS AND SIZE FOR TRAFFIC LIGHTS*/
#define TL_COL		makecol(235,216,59)
#define TL_SIZE		16
#define BL_BORDER	3
#define LIGHTS_RAY	5

/*COLORS FOR TRAFFIC LIGHTS*/
#define TL_GREEN	makecol(43, 171, 17)
#define TL_YELLOW	42
#define TL_RED		12

/*COLOR STREET LINES*/
#define SL_COL		0

/** BINARY COLOR DEFINITION **/
#define	BLACK	0
#define	WHITE	15

/** MAX CARS ALLOWED **/
#define MAX_CARS	10

/** CAR LIGHTS **/
#define CAR_RUNNING	14
#define CAR_STOPPED	makecol(255,50,50)

/* STATUS DISPLAY COLORS */
#define STAT_BG		makecol(43, 46, 73)
#define INFO_COL	makecol(247,196,12)

/* copy int elements from source to destination */
void cpyPnts(int *source, int *destination, const int N);

/* compares int elements between A and B */
char cmpPnts(int *A, int *B, const int N);

#endif