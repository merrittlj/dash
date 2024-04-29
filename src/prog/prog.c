/*
 * filename:	prog.c
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	implementation for prog.h
 */

#include <stdint.h>

#include "prog.h"
#include "hal.h"


const struct state DEFAULT_FSM[NUM_DEFAULT_STATES] = {
	[STATE_MAX_SPEED] = {
		max_speed_action,
		max_speed_transition,
		.next = STATE_TRIP_DISTANCE
	},

	[STATE_TRIP_DISTANCE] = {
		trip_distance_action,
		trip_distance_transition,
		.next = STATE_TOTAL_DISTANCE
	},

	[STATE_TOTAL_DISTANCE] = {
		total_distance_action,
		total_distance_transition,
		.next = STATE_MAX_SPEED
	}
};

void max_speed_action()
{
	gpio_set_mode(LED_RED_PIN, GPIO_MODE_OUTPUT);
	gpio_write(LED_RED_PIN, GPIO_OUTPUT_SET);
}

void max_speed_transition()
{
	gpio_write(LED_RED_PIN, GPIO_OUTPUT_CLEAR);
}

void trip_distance_action()
{
	gpio_set_mode(LED_GREEN_PIN, GPIO_MODE_OUTPUT);
	gpio_write(LED_GREEN_PIN, GPIO_OUTPUT_SET);
}

void trip_distance_transition()
{
	gpio_write(LED_GREEN_PIN, GPIO_OUTPUT_CLEAR);
}

void total_distance_action()
{
	gpio_set_mode(LED_BLUE_PIN, GPIO_MODE_OUTPUT);
	gpio_write(LED_BLUE_PIN, GPIO_OUTPUT_SET);
}

void total_distance_transition()
{
	gpio_write(LED_BLUE_PIN, GPIO_OUTPUT_CLEAR);
}
