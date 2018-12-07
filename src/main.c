#include <stdio.h>
#include <allegro.h>
#include <pthread.h>

#include "types.h"
#include "vehicle.h"
#include "utils.h"
#include "task.h"

extern pthread_mutex_t screen_lock;

void *dummy(void* arg){
	int* p;
	p = (int*) arg;
	int x;
	x = *p;
	printf("I am Dummy %d\n", x);
	printf("I am a Dumb\n");
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
	// for (int i = 0; i < 2; ++i) {
		vehicle_t myCar;
		rt_task_par_t vehicle_prm;
		task_create(initVehicle, moveVehicle, &myCar, &vehicle_prm, 200, 200, 2);

		vehicle_t myCar2;
		rt_task_par_t vehicle_prm2;
		task_create(initVehicle, moveVehicle, &myCar2, &vehicle_prm2, 200, 200, 2);

		vehicle_t myCar3;
		rt_task_par_t vehicle_prm3;
		task_create(initVehicle, moveVehicle, &myCar3, &vehicle_prm3, 200, 200, 2);

		vehicle_t myCar4;
		rt_task_par_t vehicle_prm4;
		task_create(initVehicle, moveVehicle, &myCar4, &vehicle_prm4, 200, 200, 2);

	// }
	/**********************/

	/**** Test Task *******/
	rt_task_par_t dummy_par;
	int i;
	i = 1;
	// task_create(NULL, dummy, &i, &dummy_par, 1000, 1000, 1);
	// task_create(dummy, &dummy_par, 1, 1, 99);
	/***********************/

	while(1){}

	allegro_exit();

	return 0;
}
