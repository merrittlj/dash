/*
 * filename:	prog.c
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	implementation for prog.h
 */

#include <stdint.h>

#include "prog.h"
#include "hal.h"
#include "seg.h"
#include "mag.h"


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

	seg_pins(SHIFT_SER_PIN, SHIFT_RCLK_PIN, SHIFT_SRCLK_PIN, DP1_PIN, DP2_PIN, DP3_PIN, DP4_PIN);  /* TODO: Change this. */
	seg_new(get_max_speed());
	seg_display_next();
}

void max_speed_transition()
{
	gpio_write(LED_RED_PIN, GPIO_OUTPUT_CLEAR);
}

void trip_distance_action()
{
	gpio_set_mode(LED_GREEN_PIN, GPIO_MODE_OUTPUT);
	gpio_write(LED_GREEN_PIN, GPIO_OUTPUT_SET);

	seg_new(get_trip_distance());
	seg_display_next();
}

void trip_distance_transition()
{
	gpio_write(LED_GREEN_PIN, GPIO_OUTPUT_CLEAR);
}

void total_distance_action()
{
	gpio_set_mode(LED_BLUE_PIN, GPIO_MODE_OUTPUT);
	gpio_write(LED_BLUE_PIN, GPIO_OUTPUT_SET);

	seg_new(get_max_speed());
	seg_display_next();
}

void total_distance_transition()
{
	gpio_write(LED_BLUE_PIN, GPIO_OUTPUT_CLEAR);
}

void timer_mode()
{	
	gpio_set_mode(LED_GREEN_PIN, GPIO_MODE_OUTPUT);
	gpio_set_mode(LED_RED_PIN, GPIO_MODE_OUTPUT);
	seg_pins(SHIFT_SER_PIN, SHIFT_RCLK_PIN, SHIFT_SRCLK_PIN, DP1_PIN, DP2_PIN, DP3_PIN, DP4_PIN);  /* TODO: Change this. */

	uint16_t x = 6000;
	uint8_t red_on = 0;
	seg_new_manual(x, 2);
	seg_display_next();

	uint32_t red_timer, red_period = 50;
	uint32_t seg_timer, seg_period = 2;
	uint32_t x_timer, x_period = 10;
	gpio_write(LED_RED_PIN, GPIO_OUTPUT_CLEAR);
	gpio_write(LED_GREEN_PIN, GPIO_OUTPUT_SET);
	while (gpio_read(STATEBTN_PIN)) {
		if (timer_expired(&seg_timer, seg_period, s_ticks)) {
			seg_display_next();
		}
	}
	gpio_write(LED_GREEN_PIN, GPIO_OUTPUT_CLEAR);
	gpio_write(LED_RED_PIN, GPIO_OUTPUT_SET);

	while (x > 0) {
		if (timer_expired(&red_timer, red_period, s_ticks)) {
			gpio_write(LED_RED_PIN, red_on);
			red_on ^= 1;
		}

		if (timer_expired(&seg_timer, seg_period, s_ticks)) {
			seg_display_next();
		}

		if (timer_expired(&x_timer, x_period, s_ticks)) {
			set_display_num(x);
			--x;
		}
	}
	gpio_write(LED_RED_PIN, GPIO_OUTPUT_CLEAR);
	for (;;) {;}
}
