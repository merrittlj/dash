/*
 * filename:	main.c
 * date:	03.29.24
 * author:	Lucas Merritt/merrittlj
 * description:	Program core.
 */

#include <stdint.h>

#include "main.h"  /* ONLY for main function, so startup.c doesn't include main.c */
#include "prog.h"
#include "hal.h"
#include "state.h"


int main()
{
	stk_init(6000000 / 1000);  /* TODO: this timer is incorrect. */
	uint32_t debounce_timer, debounce_period = 50;
	uint32_t idle_timer, idle_period = 10000;

	uint16_t usrbtn = PIN('A', 0);

	rcc_port_enable(PIN_BANK(usrbtn), RCC_PORT_ENABLE);
	gpio_set_mode(usrbtn, GPIO_MODE_INPUT);

	struct fsm machine;
	fsm_init(&machine, DEFAULT_FSM, STATE_MAX_SPEED);

	for (;;) {
		if (gpio_read_input(usrbtn)) {
			while (gpio_read_input(usrbtn)) (void) 0;
			while (!timer_expired(&debounce_timer, debounce_period, s_ticks)) (void) 0;

			idle_timer = 0;
			fsm_next(&machine);
		}
		if (timer_expired(&idle_timer, idle_period, s_ticks)) {
			/* TODO: proper state machine idle. */
			write_builtin_led(LED_COLOR_RED, GPIO_OUTPUT_CLEAR);
			write_builtin_led(LED_COLOR_GREEN, GPIO_OUTPUT_CLEAR);
			write_builtin_led(LED_COLOR_BLUE, GPIO_OUTPUT_CLEAR);
			write_builtin_led(LED_COLOR_ORANGE, GPIO_OUTPUT_CLEAR);
		}
	}

	return 0;
}
