#include <stdio.h>
#include <allegro.h>
#include <pthread.h>

#include "types.h"
#include "vehicle.h"
#include "tlmanager.h"
#include "utils.h"
#include "task.h"
#include <unistd.h>

extern pthread_mutex_t screen_lock;

void *dummy(void* arg){
	int* p;
	p = (int*) arg;
	int x;
	x = *p;
	printf("I am Dummy %d\n", x);
	printf("I am a Dumb\n");
}

void initialize_graphics(int *tl_matrix)
{
	set_color_depth(N_COL);

	set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, H, 0, 0);

	clear_to_color(screen, STREET_COL);
	

	/*Initializing a simple random map*/
	initRandomMap(tl_matrix);
}

int tl_matrix[N_BLOCKS_X*2 * N_BLOCKS_Y*2];

int main(int argc, char const *argv[])
{
	allegro_init();

	//n_blocks = 30;	//for now

	initialize_graphics(tl_matrix);

	/**** Testing Task/Vehicle *****/
	// for (int i = 0; i < 2; ++i) {
		vehicle_t myCar;
		rt_task_par_t vehicle_prm;
		task_create(initVehicle, moveVehicle, &myCar, &vehicle_prm, 40, 40, 2);

		// vehicle_t myCar2;
		// rt_task_par_t vehicle_prm2;
		// task_create(initVehicle, moveVehicle, &myCar2, &vehicle_prm2, 40, 40, 2);

		// vehicle_t myCar3;
		// rt_task_par_t vehicle_prm3;
		// task_create(initVehicle, moveVehicle, &myCar3, &vehicle_prm3, 40, 40, 2);

		// vehicle_t myCar4;
		// rt_task_par_t vehicle_prm4;
		// task_create(initVehicle, moveVehicle, &myCar4, &vehicle_prm4, 40, 40, 2);

	// }
	/**********************/

	/**** Testing Switch Light task *****/

	tl_manager_arg man_arg;
	man_arg.tl_matrix = tl_matrix;
	rt_task_par_t tl_manager_par;
	// sleep(4);
	task_create(&tl_init, &tl_manager, &man_arg, &tl_manager_par, 4000, 1000, 3);

	/************************************/

	while(1){}

	allegro_exit();

	return 0;
}
