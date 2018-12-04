#ifndef UTILS_H
#define UTILS_H value

/*RESOLUTION CONSTANTS*/
#define W	1024
#define H	768
/*BITS FOR COLORS*/
#define N_COL	8
/*COLORS USED FOR THE MAP*/
#define STREET_COL	15
#define BLOCK_COL	2
/*STANDARD DIMENSIONS OF THE STREETS AND BLOCKS*/
#define BLOCK_W 	40
#define STREET_W 	40

void initGridMap(const int street_w, const int block_w);
/* Inits grid map
 */

void initRandomMap(const int street_w);
/* Inits a random map
 */

#endif