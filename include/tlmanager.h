#ifndef TL_MANAGER_H
#define TL_MANAGER_H

typedef struct{
	int *tl_matrix;
} tl_manager_arg;

void tl_manager(void * arg);

void tl_init();

#endif