/*
 * filename:	prog.h
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	program-specific implementations
 */

#ifndef PROG_H
#define PROG_H

#include "hal.h"
#include "state.h"


/* Pins */
#define STATEBTN_PIN (PIN('C', 13))
#define LED_RED_PIN (PIN('B', 5))
#define LED_GREEN_PIN (PIN('B', 4))
#define LED_BLUE_PIN (PIN('B', 3))
#define SHIFT_SER_PIN (PIN('C', 10))
#define SHIFT_RCLK_PIN (PIN('A', 15))
#define SHIFT_SRCLK_PIN (PIN('A', 8))
#define DP1_PIN (PIN('B', 6))
#define DP2_PIN (PIN('B', 7))
#define DP3_PIN (PIN('B', 8))
#define DP4_PIN (PIN('B', 9))
#define STATUS_PIN (PIN('C', 11))
#define HALL_SENSOR_PIN (PIN('B', 2))

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

/* Easter eggs: */
extern void timer_mode();

#endif
