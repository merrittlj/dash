/*
 * filename:	prog.h
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	program-specific implementations
 */

#ifndef PROG_H
#define PROG_H
#include <stdint.h>

#include "state.h"


/* LED pin #'s. */
enum {LED_COLOR_RED = 6, LED_COLOR_BLUE, LED_COLOR_ORANGE, LED_COLOR_GREEN};

/* Write LED color on port to mode. */
extern void write_builtin_led(uint8_t color, uint8_t mode);

/* States for the default FSM. */
enum {STATE_MAX_SPEED, STATE_TRIP_DISTANCE, STATE_TOTAL_DISTANCE, NUM_DEFAULT_STATES};

extern const struct state DEFAULT_FSM[NUM_DEFAULT_STATES];

/* Default FSM actions and transitions: */
extern void max_speed_action();
extern void max_speed_transition();
extern void trip_distance_action();
extern void trip_distance_transition();
extern void total_distance_action();
extern void total_distance_transition();

#endif
