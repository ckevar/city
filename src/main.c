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

int main(int argc, char const *argv[])
{
	vehicle_t 		cars[MAX_CARS];
	rt_task_par_t 	carsPrms[MAX_CARS];
	char scan;
	char carCounter = 0;
	char isPressed = 1;

	ShouldISuicide = 0;

	srand (time(NULL));
	allegro_init();

	initialize_graphics(tl_matrix);

	/**** Undet Test Switch Light task *****/	
	tl_manager_arg man_arg;
	man_arg.tl_matrix = tl_matrix;
	rt_task_par_t tl_manager_par;
	task_create(&tl_init, &tl_manager, NULL, &man_arg, &tl_manager_par, 4000, 1000, 3);
	/***************************************/

	/******* Under Test Vehicle-Task *******/
	do {

		if(keypressed()) {				// if there's is a pressed key,  
			scan = readkey() >> 8;		// it catches the pressed key,
			isPressed++;				// How many times the key is pressed
		} else isPressed = 0;			// otherwise no key pressed

		if (isPressed == 1) {			// only the first catch will be taken in count

			/** If N key is pressed a new car will be created only if MAX is not reached **/
			if (scan == KEY_N && carCounter < MAX_CARS) {
				task_create(initVehicle, moveVehicle, termVehicle, &cars[carCounter], &carsPrms[carCounter], 40, 40, 2);
				carCounter++;
			}

			/** if D key is pressed a car will be removed off simulation **/
			if (scan == KEY_D && carCounter > 0) {
				if(task_terminate(&carsPrms[carCounter - 1]))
					fprintf(stderr, "Error while stopping a vehicle task\n");
				else
					carCounter--;
			}
		}

	} while(scan != KEY_ESC);
	/***************************************/

	ShouldISuicide = 1;		// to kill every periodic tasks

	allegro_exit();

	return 0;
}
