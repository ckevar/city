#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>

#include "task.h"
#include "timemanagement.h"
#include "graphics.h"

/*Checks if there's a deadline miss*/
int deadline_miss(rt_task_par_t * parameters){
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (time_cmp(now, parameters->dl) > 0) {
		parameters->dmiss++;
		return 1;
	}
	return 0;
}

/*Sets next activation time of a task*/
void set_activation(rt_task_par_t * parameters){
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	time_copy(&(parameters->at), t);
	time_copy(&(parameters->dl), t);
	time_add_ms(&(parameters->at), parameters->period);		//next activation time = now + period
	time_add_ms(&(parameters->dl), parameters->deadline);	//next deadline = now + relative deadline
}

/*Suspends the calling thread until next activation*/
void wait_for_activation(rt_task_par_t * parameters){
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
	 					&(parameters->at), NULL);		//sleeps until next activation
	time_add_ms(&(parameters->at), parameters->period);	//next activation time = old act time + period
	time_add_ms(&(parameters->dl), parameters->period);	//next deadline = old deadline + period
}

/*Periodic task management*/
void *periodic_task(void* arg) {
	rt_task_par_t * prm = (rt_task_par_t *) arg;
	set_activation(prm);
	prm->init(prm->arg); 			// Inits the ARG entity

	while(!(prm->killMyself)){
		prm->run(prm->arg);			// Runs

		if(deadline_miss(prm)){
			fprintf(stderr, "[WARNING:] Deadline Miss\n");
			drawNDmiss();
		}
			
		wait_for_activation(prm);
	}
	if(prm->terminate != NULL)
		prm->terminate(prm->arg);	/* termination function */
}

int task_create(void *init, void *run, void *term, void *arg, rt_task_par_t *par, int period, int deadline, int priority){

	pthread_attr_t myattr;
	struct sched_param mypar;
	int tret;

	if(priority > 99){
		priority = 99;
		fprintf(stderr, "Cannot assign a priority higher than 99, assigning 99 to the task priority \n");
	}
	if(priority < 0){
		priority = 0;
		fprintf(stderr, "Cannot assign a priority smaller than 0, assigning 0 to the task priority \n");
	}
	// (*init)(arg);

	/*setting up the parameters of the task from input*/
	par->isActive	=	1;
	par->dmiss 		=	0;
	par->killMyself	=	0;
	par->init 		= 	init;
	par->run 		=	run;
	par->terminate	=	term;
	par->arg		=	arg;
	par->period 	=	period;
	par->deadline 	=	deadline;
	par->priority 	=	priority;
	
	pthread_attr_init(&myattr);

	/*notifying different scheduling from parent: */
	pthread_attr_setinheritsched(&myattr, PTHREAD_EXPLICIT_SCHED);

	/*setting the scheduler (for now fixed-pr. + RR): */
	pthread_attr_setschedpolicy(&myattr, SCHED_RR);

	mypar.sched_priority = par->priority;
	/*setting thread priority*/
	pthread_attr_setschedparam(&myattr, &mypar);

	tret = pthread_create(&(par->tid), &myattr, periodic_task, par);
	if(tret)
		fprintf(stderr, "Error while creating a new task! %d\n", tret);

	return tret;
}

/*	Terminates a periodic task that is executing
*	It first ssignals to the thread to suicide through
*	its real time parameters struct
*	and then calls a termination function 	
*/
void task_terminate(rt_task_par_t *par){
	par->killMyself = 1;	/* tells the thread to suicide */
	pthread_join(par->tid, NULL);
	par->isActive = 0;
}
