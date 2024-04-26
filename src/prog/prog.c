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

struct button_data bd;

void write_builtin_led(uint8_t color, uint8_t mode)
{
	gpio_set_mode(PIN('C', color), GPIO_MODE_OUTPUT);
	gpio_write(PIN('C', color), mode);
}

void button_vars_init(uint16_t button, struct fsm *machine, uint32_t *debounce_timer, uint32_t debounce_period, uint32_t *idle_timer, uint32_t idle_period)
{
	bd.button = button;
	bd.machine = machine;
	bd.debounce_timer = debounce_timer;
	bd.debounce_period = debounce_period;
	bd.idle_timer = idle_timer;
	bd.idle_period = idle_period;
}

void button_handler()
{
	*bd.idle_timer = s_ticks + bd.idle_period;
	fsm_next(bd.machine);

	while (gpio_read(bd.button)) (void) 0;
	while (!timer_expired(bd.debounce_timer, bd.debounce_period, s_ticks)) (void) 0;
}

void max_speed_action()
{
	write_builtin_led(LED_COLOR_RED, GPIO_OUTPUT_SET);
}

void max_speed_transition()
{
	write_builtin_led(LED_COLOR_RED, GPIO_OUTPUT_CLEAR);
}

void trip_distance_action()
{
	write_builtin_led(LED_COLOR_GREEN, GPIO_OUTPUT_SET);
}

void trip_distance_transition()
{
	write_builtin_led(LED_COLOR_GREEN, GPIO_OUTPUT_CLEAR);
}

void total_distance_action()
{
	write_builtin_led(LED_COLOR_BLUE, GPIO_OUTPUT_SET);
}

void total_distance_transition()
{
	write_builtin_led(LED_COLOR_BLUE, GPIO_OUTPUT_CLEAR);
}
