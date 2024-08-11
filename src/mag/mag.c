/*
 * filename:	mag.c
 * date:	08.10.24
 * author:	Lucas Merritt/merrittlj
 * description:	Implementation for mag.h
 */

#include <stdint.h>
#include "mag.h"

#include "hal.h"
#include "prog.h"


static double max_speed;  /* Max speed with two decimals precision, stored as xxxx instead of xx.xx */
static uint16_t trip_pulses;  /* Total pulses(magnet passes) this trip */
static uint16_t since_pulse;  /* ms since last pulse */
static uint8_t prev_mag_present;  /* bool if last read was magnet */

void init_mag()
{
	max_speed = 0;
	trip_pulses = 0;
	since_pulse = 0;
	prev_mag_present = 0;
}

void mag_interrupt()
{
	uint8_t magnet = gpio_read(HAL_SENSOR_PIN);

	if (!magnet) ++since_pulse;
	else if (magnet && !prev_mag_present && since_pulse != 0)
	{
		++trip_pulses;
		double current_speed = ((WHEEL_CIRC / WHEEL_MAGNETS) / ((double)since_pulse / 1000)) / 44.704;
		if (current_speed > max_speed) max_speed = current_speed;
		since_pulse = 0;
	}

	prev_mag_present = magnet;
}

double get_max_speed()
{
	return max_speed;
}

double get_trip_distance()
{
	/* Find revolutions of wheel, multiply by wheel circ. */
	return ((trip_pulses / WHEEL_MAGNETS) * WHEEL_CIRC);  /* TODO: this doesn't work.. */
}

static double read_saved_distance()
{
	return 50;
}

double get_total_distance()
{
	return get_trip_distance() + read_saved_distance();
}
