#ifndef GRAPHICS_H
#define GRAPHICS_H

/* Inits the graphical environment */
void initialize_graphics(int *tl_matrix);

/* Inits grid map */
void initGridMap(int *tl_matrix, const int street_w, const int block_w);

/* Inits a random map */
void initRandomMap(int* tl_matrix);

#endif
