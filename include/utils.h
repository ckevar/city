#ifndef UTILS_H
#define UTILS_H value

/*RESOLUTION CONSTANTS*/
#define W 640
#define H 480
/*BITS FOR COLORS*/
#define N_COL 8

void initGridMap(const char n_blocks);
/* Inits grid map
 */

void initRandomMap(const int street_w, const int numRoads);
/* Inits a random map, calls initGrid and fills some blocks
 */

#endif