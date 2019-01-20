#include <allegro.h>
#include <stdio.h>
#include <pthread.h>

#include "utils.h"
#include "tlgraphics.h"
#include "vehicle.h"

pthread_mutex_t dmiss_lock;
int deadline_misses;

/*Initializes a regular grid with specified dimensions*/
void initGridMap(int *tl_matrix, const int street_w, const int block_w){
	char n_blocks_x, n_blocks_y;
	int i, j;

	int someCoeff = block_w + street_w;

	n_blocks_x = (W - street_w) / (block_w + street_w);
	n_blocks_y = (H - street_w) / (block_w + street_w);

	for(i = 0; i < n_blocks_x; i++){
		for(j = 0; j < n_blocks_y; j++){
			rectfill(screen, (i * someCoeff) + street_w, 
							(j * someCoeff) + street_w, 
							(i * someCoeff) + block_w + street_w, 
							(j * someCoeff) + block_w + street_w, BLOCK_COL);
			drawTLCabins(tl_matrix, i, j, someCoeff);
		}
	}
}

/*Initializes a random grid with specified dimensions*/
void initRandomMap(int *tl_matrix){

	int current_blocks = 0;
	int i, j;

	int someCoeff = BLOCK_W + STREET_W;
	int random_factor;
	int a, b;


	for(i = 0; i < N_BLOCKS_X; i++){
		for(j = 0; j < N_BLOCKS_Y; j++){

			random_factor = rand() % 40;
			if(random_factor >= 50 || (i == N_BLOCKS_X - 1) || (j == N_BLOCKS_Y - 1)) {
				a = 1; b = 1;
			}
			else if(random_factor >= 25){
				a = 2; b = 1;
			}
			else{
				a = 1; b = 2;
			}
			rectfill(screen, (i * someCoeff) + STREET_W, 
							 (j * someCoeff) + STREET_W, 
							 (i * someCoeff) + BLOCK_W + a * STREET_W, 
							 (j * someCoeff) + BLOCK_W + b * STREET_W, BLOCK_COL);


		}
	}

	for (i = 0; i < N_BLOCKS_X; i++) {
		for (j = 0; j < N_BLOCKS_Y; j++) {
			drawTLCabins(tl_matrix, i, j, someCoeff);
		}
	}

}

/* Prints the specified number of existing cars in the map */
void drawNCars(int x){
	char s[2];
	int color;

	snprintf(s, sizeof(int), "%d", x);

	if(x <= (MAX_CARS/3))
		color = INFO_GREEN;
	else if(x <= (2*MAX_CARS/3))
		color = INFO_YEL;
	else
		color = INFO_RED;

	rectfill(screen, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 50, 47,
		(N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 70, 60, STAT_BG);
	textout_centre_ex(screen, font, s, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		50, color, -1);
}

/* Prints the specified reference velocity of existing cars in the city */
void drawGVel(double vel) {
	char s[2];
	int color;
	snprintf(s, sizeof(int), "%d", (int)vel);

	if(vel <= (MAX_VEL_CAR / 3)) 
		color = INFO_GREEN;
	else if (vel <= (2 * MAX_CARS) / 3)
		color = INFO_YEL;
	else
		color = INFO_RED;

	rectfill(screen, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 50, 248,
		(N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 70, 260, STAT_BG);
	textout_centre_ex(screen, font, s, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		250, color, -1);

}


/* Sets up the display and interface*/
void initStatDisplay(){
	// char* desc, insert, delete;
	const char *desc	= "WELCOME IN CITY, A SIMULATION PROGRAM THAT AIMS TO SIMULATE A URBAN AREA.";
	const char *insert	= "TO ADD A NEW CAR, PRESS 'n'.";
	const char *delete	= "TO DELETE AN EXISTING CAR, PRESS 'd'.";
	const char *incVel 	= "TO INCREASE VELOCITY, PRESS UP KEY";
	const char *decVel 	= "TO DECREASE VELOCITY, PRESS DOWN KEY";
	const char *nCars1	= "NUMBER OF";
	const char *nCars2	= "CARS";
	const char *nCars3	= "IN THE CITY:";
	const char *nDmiss1	= "NUMBER OF";
	const char *nDmiss2 = "DEADLINE";
	const char *nDmiss3	= "MISSES:";
	const char *nGVel1	= "GLOBAL";
	const char *nGVel2	= "VELOCITY";
	int dmiss = 0;
	char s[4];

	rectfill(screen, 0, H, W, N_BLOCKS_Y*BLOCK_W + (N_BLOCKS_Y + 1)*STREET_W, STAT_BG);
	rectfill(screen, N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X + 1)*STREET_W, 0, W, H, STAT_BG);

	textout_centre_ex(screen, font, "CITY", (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W)/2,
		(N_BLOCKS_Y*BLOCK_W + (N_BLOCKS_Y+1)*STREET_W+5), INFO_COL, -1);
	textout_centre_ex(screen, font, desc, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W)/2,
		(N_BLOCKS_Y*BLOCK_W + (N_BLOCKS_Y+1)*STREET_W+15), INFO_COL, -1);
	textout_ex(screen, font, insert, 20, (N_BLOCKS_Y*BLOCK_W + (N_BLOCKS_Y+1)*STREET_W+35), INFO_GREEN, -1);
	textout_ex(screen, font, delete, 20, (N_BLOCKS_Y*BLOCK_W + (N_BLOCKS_Y+1)*STREET_W+45), INFO_RED, -1);
	textout_ex(screen, font, incVel, 20, (N_BLOCKS_Y*BLOCK_W + (N_BLOCKS_Y+1)*STREET_W+55), INFO_RED-1, -1);
	textout_ex(screen, font, decVel, 20, (N_BLOCKS_Y*BLOCK_W + (N_BLOCKS_Y+1)*STREET_W+65), INFO_RED-2, -1);

	textout_centre_ex(screen, font, nCars1, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		20, INFO_COL, -1);
	textout_centre_ex(screen, font, nCars2, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		30, INFO_COL, -1);
	textout_centre_ex(screen, font, nCars3, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		40, INFO_COL, -1);
	textout_centre_ex(screen, font, nDmiss1, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		130, INFO_COL, -1);
	textout_centre_ex(screen, font, nDmiss2, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		140, INFO_COL, -1);
	textout_centre_ex(screen, font, nDmiss3, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		150, INFO_COL, -1);

	textout_centre_ex(screen, font, nGVel1, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		230, INFO_COL, -1);
	textout_centre_ex(screen, font, nGVel2, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		240, INFO_COL, -1);

	drawNCars(0);
	drawGVel(0);

	snprintf(s, sizeof(int), "%d", dmiss);
	textout_centre_ex(screen, font, s, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		160, INFO_GREEN, -1);
}


void initialize_graphics(int *tl_matrix)
{
	set_color_depth(N_COL);

	set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, H, 0, 0);

	clear_to_color(screen, STREET_COL);
	install_keyboard();
	

	/*Initializing a simple random map*/
	initRandomMap(tl_matrix);
	initStatDisplay();
}


/* Prints the number of deadline-misses in the interface */
void drawNDmiss(){

	char s[6];
	int color;

	rectfill(screen, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 40, 157,
		(N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 80, 170, STAT_BG);

	/* Critical section on the shared var deadline_misses */
	pthread_mutex_lock(&dmiss_lock);


	deadline_misses++;

	if(deadline_misses <= 10)
		color = INFO_GREEN;
	else if(deadline_misses <= 20)
		color = INFO_YEL;
	else
		color = INFO_RED;

	snprintf(s, 6, "%d", deadline_misses);
	textout_centre_ex(screen, font, s, (N_BLOCKS_X*BLOCK_W + (N_BLOCKS_X+ 1)*STREET_W) + 60,
		160, color, -1);


	pthread_mutex_unlock(&dmiss_lock);
	/* End of critical section */
}