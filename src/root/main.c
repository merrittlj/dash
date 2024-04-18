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
	uint16_t usrbtn = PIN('A', 0);

	rcc_port_enable(PIN_BANK(usrbtn), RCC_PORT_ENABLE);
	gpio_set_mode(usrbtn, GPIO_MODE_INPUT);

	struct fsm machine;
	fsm_init(&machine, DEFAULT_FSM, STATE_MAX_SPEED);

	for (;;) {
		if (gpio_read_input(usrbtn)) {
			fsm_next(&machine);

			/* Wait until button is not pressed. */
			while (gpio_read_input(usrbtn)) (void) 0;
		}
	}

	return 0;
}
