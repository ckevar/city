#include <stdio.h>
#include <allegro.h>

#include "types.h"
#include "vehicle.h"
#include "utils.h"
#include "task.h"

void *dummy(void* arg){
	int* p;
	p = (int*) arg;
	int x;
	x = *p;
	printf("I am Dummy %d\n", x);
}

void initialize_graphics()
{
	set_color_depth(N_COL);

	set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, H, 0, 0);

	clear_to_color(screen, STREET_COL);

	/*Initializing a simple random map*/
	initRandomMap(STREET_W);
}

int main(int argc, char const *argv[])
{
	allegro_init();

	//n_blocks = 30;	//for now

	initialize_graphics();

	/**** Testing Vehicle *****/
	vehicle_t v1;
	initVehicle(&v1, W, H);
	moveVehicle(&v1);
	getFrame(&v1);
	/**********************/

	/**** Test Task *******/
	rt_task_par_t dummy_par;
	int i;
	i = 1;
	task_create(dummy, &i, &dummy_par, 1, 1, 99);
	// task_create(dummy, &dummy_par, 1, 1, 99);
	/***********************/

	while(1){}

	allegro_exit();

	return 0;
}
