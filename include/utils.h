#ifndef UTILS_H
#define UTILS_H

/*RESOLUTION CONSTANTS*/
#define W	1920
#define H	1080

/*BITS FOR COLORS*/
#define N_COL	8

/* MAXIMUN KNOWN ENCODED COLORS */
#define MAX_KNOWN_ENCODED_COLORS 15

/*COLORS USED FOR THE MAP*/
#define STREET_COL	15
#define BLOCK_COL	2

/*STANDARD DIMENSIONS OF THE STREETS AND BLOCKS*/
#define BLOCK_W 	100
#define STREET_W 	30

void initGridMap(const int street_w, const int block_w);
/* Inits grid map
 */

void initRandomMap(const int street_w);
/* Inits a random map
 */

#endif