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
	stk_init(8000000 / 1000);
	uint32_t debounce_timer, debounce_period = 50;
	uint32_t idle_timer, idle_period = 10000;

	uint16_t usrbtn = PIN('A', 0);

	rcc_port_enable(PIN_BANK(usrbtn), RCC_PORT_ENABLE);
	gpio_set_mode(usrbtn, GPIO_MODE_INPUT);

	/* exti_pin_init(usrbtn, 1, 0, button_handler); */
	exti_simple_init(button_handler);

	struct fsm machine;
	fsm_init(&machine, DEFAULT_FSM, STATE_MAX_SPEED);

	button_vars_init(usrbtn, &machine, &debounce_timer, debounce_period, &idle_timer);
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
