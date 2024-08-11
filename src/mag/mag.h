#ifndef MAG_H
#define MAG_H

#include <stdint.h>


/* # in the holder */
#define WHEEL_MAGNETS 8
/* circ in cm */
#define WHEEL_CIRC 80

extern void init_mag();
extern void mag_interrupt();  /* Called every 1ms by SysTick */
/* Both return xxxx instead of xx.xx */
extern double get_max_speed();
extern double get_trip_distance();
extern double get_total_distance();


#endif
