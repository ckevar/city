#ifndef GRAPHICS_H
#define GRAPHICS_H

/* Inits the graphical environment */
void initialize_graphics(int *tl_matrix);

/* Prints the specified number of existing cars in the map */
void drawNCars(int x);

/* Prints the number of deadline-misses in the interface */
void drawNDmiss();

/* Prints the specified reference velocity of existing cars in the city */
void drawGVel(double vel);

#endif
