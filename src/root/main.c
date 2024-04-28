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
	uint32_t debounce_timer, debounce_period = 20;
	uint32_t idle_timer, idle_period = 10000;

	uint16_t usrbtn = PIN('C', 13);
	uint8_t button_pressed = 0;
	gpio_set_mode(usrbtn, GPIO_MODE_INPUT);

	struct fsm machine;
	fsm_init(&machine, DEFAULT_FSM, STATE_MAX_SPEED);

	NVIC_SetPriority (SysTick_IRQn, 0);
	for (;;) {
		/* To prevent debounce, simply poll the button every 50ms or so. */
		if (timer_expired(&debounce_timer, debounce_period, s_ticks)) {
			if (!gpio_read(usrbtn) && !button_pressed) {  /* Button input is pull-up. */
				button_pressed = 1;

				idle_timer = s_ticks + idle_period;
				fsm_next(&machine);
			} else if (gpio_read(usrbtn)) button_pressed = 0;
		}
		if (timer_expired(&idle_timer, idle_period, s_ticks)) {
			gpio_write(PIN('C', 14), GPIO_OUTPUT_CLEAR);
			gpio_write(PIN('C', 15), GPIO_OUTPUT_CLEAR);
			gpio_write(PIN('F', 0), GPIO_OUTPUT_CLEAR);
		}
	}

	return 0;
}
