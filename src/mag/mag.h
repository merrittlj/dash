#ifndef MAG_H
#define MAG_H

#include <stdint.h>
#include <math.h>


/* # in the holder */
#define WHEEL_MAGNETS 8
/* diameter in cm */
#define WHEEL_DIAMETER 60
#define WHEEL_CIRC (WHEEL_DIAMETER * M_PI)
#define MAG_CIRC (WHEEL_CIRC / WHEEL_MAGNETS)

extern void init_mag();
extern void mag_interrupt();  /* Called every 1ms by SysTick */
/* Both return xxxx instead of xx.xx */
extern double get_max_speed();
extern double get_trip_distance();
extern double get_total_distance();


#endif
