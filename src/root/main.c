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
#include "seg.h"


int main()
{
	uint32_t debounce_timer, debounce_period = 20;
	uint32_t idle_timer, idle_period = 10000;
	uint8_t is_idle = 0;
	uint32_t seg_timer, seg_period = 1000;

	uint8_t button_pressed = 0;
	gpio_set_mode(STATEBTN_PIN, GPIO_MODE_INPUT);

	struct fsm machine;
	fsm_init(&machine, DEFAULT_FSM, STATE_MAX_SPEED);
	
	gpio_set_mode(SHIFT_SER_PIN, GPIO_MODE_OUTPUT);
	gpio_set_mode(SHIFT_RCLK_PIN, GPIO_MODE_OUTPUT);
	gpio_set_mode(SHIFT_SRCLK_PIN, GPIO_MODE_OUTPUT);
	
	seg_pins(SHIFT_SER_PIN, SHIFT_RCLK_PIN, SHIFT_SRCLK_PIN);
	seg_clear_output();
	
	NVIC_SetPriority (SysTick_IRQn, 0);
	uint8_t x = 0;
	for (;;) {
		/* To prevent debounce, simply poll the button every 50ms or so. */
		if (timer_expired(&debounce_timer, debounce_period, s_ticks)) {
			if (!gpio_read(STATEBTN_PIN) && !button_pressed) {  /* Button input is pull-up. */
				button_pressed = 1;
				/* Capture first button press and re-do the action. */
				if (is_idle) {
					is_idle = 0;
					fsm_action(&machine);
					continue;
				}

				idle_timer = s_ticks + idle_period;
				fsm_next(&machine);
			} else if (gpio_read(STATEBTN_PIN)) button_pressed = 0;
		}
		if (timer_expired(&idle_timer, idle_period, s_ticks) && !is_idle) {
			is_idle = 1;
			gpio_write(LED_RED_PIN, GPIO_OUTPUT_CLEAR);
			gpio_write(LED_GREEN_PIN, GPIO_OUTPUT_CLEAR);
			gpio_write(LED_BLUE_PIN, GPIO_OUTPUT_CLEAR);
			seg_clear_output();
		}
		if (timer_expired(&seg_timer, seg_period, s_ticks)) {
			seg_clear_output();
			seg_display_digit(x, 0);
			++x; x = x % 10;
		}
	}

	return 0;
}
