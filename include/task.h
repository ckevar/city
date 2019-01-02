#include <pthread.h>

#ifndef TASK_H
#define TASK_H

/** THREAD ENDERS **/
int ShouldISuicide;


/*Structure that defines the parameters of a real time task*/
typedef struct {
	pthread_t tid;				/* task id*/
	// long wcet;					/* in microseconds */
	int period;					/* in milliseconds */
	int deadline;				/* relative (ms) */
	int priority;				/* in [0,99] */
	int dmiss;					/* no. of misses */
	struct timespec at;			/* next activ. time */
	struct timespec dl;			/* abs. deadline */
	void (*init)(void *a);		/* task setup to be executed */
	void (*run)(void *a);		/* task running to be executed */
	void (*terminate)(void *a);	/* task termination function */
	void *arg;					/* argument of the function */
	// void *t_arg;				/* argument of the termination function */
	int killMyself;				/* variable that shows if the thread should suicide */
} rt_task_par_t;

/*	Creates a task executing the first setup function (init)
*	And periodically executing the function run
*	With period expressed in milliseconds
*	Returns 0 on success, error code otherwise
*/
int task_create(void *init, void *run, void *term, void *arg, rt_task_par_t *par, int period, int deadline, int priority);

/*	Terminates a periodic task that is executing
*/
void task_terminate(rt_task_par_t *par);

#endif
