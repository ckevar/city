#ifndef TYPE_H
#define TYPE_H 

#define NUM_BEAMS 4
#define HRES 80
#define VRES 60

#define MAX_ELEMNTS	255	// Max num of elements

typedef enum tlState_ {RED_LIGHT, ORANGE_LIGHT, GREEN_LIGHT} tlState_t;
typedef enum steer_ {STEER_LEFT, STEER_RIGHT, DONT_STEER, TURN_180} steer_t;

typedef struct {
	int x[MAX_ELEMNTS];	// Coordinates along x
	int y[MAX_ELEMNTS];	// Coordinates along y
	int N;				// Number of elements being used 
} imxy_t;

typedef struct {
	int 	TLstatus;		// the traffic light color
	int 	TLminDistance;	// distance to the closest traffic light
	imxy_t	TLcenter;		// Center of the traffic lights, if any
	imxy_t 	streetCorner;	// Coordinates of corners, if any
} imfeatures_t;

typedef struct { // disjoint-set data structure
	int parent;
	int child;
} disjoint_t;

typedef struct  
{
	char	visible;
	int		x[NUM_BEAMS];		// x pixel position
	int 	y[NUM_BEAMS];		// y pixel position
	int 	stp;				// Step of the sensor
	double 	phi[NUM_BEAMS];		// angle orientation
	double 	dsts[NUM_BEAMS];	// distances measure
} dstsens_t;

typedef struct 
{
	unsigned	resH;				// Horizontal Resolution
	unsigned	resV;				// Vertical Resolution
	unsigned	x;					// x position
	unsigned	y;					// y position
	unsigned 	image[VRES][HRES];	// image
} cam_t;

typedef struct 
{
	
} gps_t;

typedef struct {
	double Kp;		// Proportional Gain
	double Ki;		// Integral Gain
	double Kd;		// Derivative Gain
} pidk_t;

typedef struct {
	int			color;
	int			xr;	 		// real (or granted) position
	int			yr; 		// real (or granted) position
	int 		w;			// Vehicle dimensions width
	int			l;			// Vehicle dimensions length
	double		m;			// Mass
	double		b;			// Friction Coefficient
	double 		u;			// Control Law
	double		theta;		// Angle Orientation
	double		vel;		// Current Velocity
	double		v_1;		// Previous Velocity
	double		Vr;			// Reference Speed
	double 		e;			// error	
	double		dx;			// delta distance
	double 		T;			// Period
	double 		Ie;			// Integral Error
	int 		point[8];	// Corners of the vehicle to be draw
	dstsens_t	ds;			// distance sensor mounted on the vehicle
	cam_t 		cam;		// camera mounted on the vehicle
	gps_t 		gps;		// gps mounted on the vehicle
	pidk_t		K;			// cruice controller 
} vehicle_t;

#endif
