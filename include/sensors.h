#ifndef SENSORS_H
#define SENSORS_H 

typedef struct 
{
	char	visible;
	int		posX;
	int 	posY;
	double 	*distances;
} dstsens_t;

typedef struct
{
	int		resX;
	int		ResolutionY;
	double	posX;
	double	posY;
} cam_t;

typedef struct
{
	
} gps_t;


#endif