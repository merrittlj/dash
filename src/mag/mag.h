#ifndef MAG_H
#define MAG_H

#include <math.h>


/* # in the holder */
#define WHEEL_MAGNETS 2
/* diameter in mm */
#define WHEEL_DIAMETER 90
#define WHEEL_CIRC (WHEEL_DIAMETER * M_PI)
#define MAG_CIRC (WHEEL_CIRC / WHEEL_MAGNETS)  /* "distance"/circumference between two consecutive magnets */

extern void init_mag();
extern void mag_interrupt();  /* Called 5/ms(0.2ms) by SysTick */

extern double get_trip_pulses();
extern double get_max_speed();
extern double get_trip_distance();
extern double get_total_distance();


#endif
