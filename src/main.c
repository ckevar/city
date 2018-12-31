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
extern int ShouldISuicide;

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
	install_keyboard();
	

	/*Initializing a simple random map*/
	initRandomMap(tl_matrix);
}

int tl_matrix[N_BLOCKS_X*2 * N_BLOCKS_Y*2];

void fillCarArray(vehicle_t *cars, rt_task_par_t *carsPrms) {
	int i;
	for (i = 0; i < MAX_CARS; ++i) {
		vehicle_t carX;
		rt_task_par_t carsPrmsX;	
		cars[i] = carX;
		carsPrms[i] = carsPrmsX;
	}
}

int main(int argc, char const *argv[])
{
	vehicle_t cars[MAX_CARS];
	rt_task_par_t carsPrms[MAX_CARS];
	int carCounter = 0;
	char scan;

	ShouldISuicide = 0;

	srand (time(NULL));
	allegro_init();

	initialize_graphics(tl_matrix);

	/**** Testing Switch Light task *****/
	tl_manager_arg man_arg;
	man_arg.tl_matrix = tl_matrix;
	rt_task_par_t tl_manager_par;
	task_create(&tl_init, &tl_manager, &man_arg, &tl_manager_par, 4000, 1000, 3);
	/************************************/

	/**** Testing Task/Vehicle *****/
	do {
		if(keypressed()) scan = readkey() >> 8;
		if (scan == KEY_SPACE && carCounter < MAX_CARS) {
			// task_create(initVehicle, moveVehicle, &cars[carCounter], &carsPrms[carCounter], 40, 40, 2);
			// carCounter++;
			// vehicle_t myCar;
			// rt_task_par_t vehicle_prm;
			// task_create(initVehicle, moveVehicle, &myCar, &vehicle_prm, 40, 40, 2);
			printf("you pressed N %d\n", carCounter++);
		}
	} while(scan != KEY_ESC);

	/**********************/

	ShouldISuicide = 1;

	allegro_exit();

	return 0;
}
