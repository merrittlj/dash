/*
 * filename:	main.c
 * date:	03.29.24
 * author:	Lucas Merritt/merrittlj
 * description:	Program core.
 */

#include <stdint.h>

#include "stm32f072xb.h"

#include "main.h"
#include "prog.h"
#include "hal.h"
#include "state.h"


int main()
{
	uint32_t debounce_timer, debounce_period = 50;
	uint32_t idle_timer, idle_period = 10000;

	uint16_t usrbtn = PIN('A', 0);

	gpio_set_mode(usrbtn, GPIO_MODE_INPUT);

	exti_pin_init(usrbtn, 1, 255, button_handler);

	struct fsm machine;
	fsm_init(&machine, DEFAULT_FSM, STATE_MAX_SPEED);

	button_vars_init(usrbtn, &machine, &debounce_timer, debounce_period, &idle_timer, idle_period);

	NVIC_SetPriority (SysTick_IRQn, 0);
	for (;;) {
		if (timer_expired(&idle_timer, idle_period, s_ticks)) {
			write_builtin_led(LED_COLOR_RED, GPIO_OUTPUT_CLEAR);
			write_builtin_led(LED_COLOR_GREEN, GPIO_OUTPUT_CLEAR);
			write_builtin_led(LED_COLOR_BLUE, GPIO_OUTPUT_CLEAR);
			write_builtin_led(LED_COLOR_ORANGE, GPIO_OUTPUT_CLEAR);
		}
	}

	return 0;
}
