/*
 * filename:	hal.c
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	implementation for hal.h
 */

#include <stdint.h>

#include "hal.h"
#include "common.h"


volatile uint32_t s_ticks = 0;

func_ptr line_funcs[32];

void gpio_set_mode(uint16_t pin, uint8_t mode)
{
	struct gpio *b = GPIO(PIN_BANK(pin));
	uint8_t n = PIN_NUM(pin);

	// 2 bits for each pin so we multiply the pin # by 2.
	b->MODER &= (uint32_t)~(3 << (n * 2));  /* Clear pin mode. */
	b->MODER |= (mode & 3) << (n * 2);  /* Set pin mode("& 3" ensures bounds). */
}

void gpio_write_output(uint16_t pin, uint8_t mode)
{
	GPIO(PIN_BANK(pin))->BSRR |= BIT(((mode ^ 1) * 16) + PIN_NUM(pin));
}

uint8_t gpio_read_input(uint16_t pin)
{
	return (GPIO(PIN_BANK(pin))->IDR >> PIN_NUM(pin)) & 1;
}

void rcc_port_enable(uint8_t bank, uint8_t mode)
{
	int n = (17 + bank);
	RCC->AHBENR = (RCC->AHBENR & ~BIT(n)) | (mode << n);
}

void stk_init(uint32_t reload)
{
	if ((reload - 1) > 0xffffff) return;  /* We have 24 bits for RVR. */
	STK->CSR = BIT(2) | BIT(1) | BIT(0);  /* Use processor clock(2), assert SysTick exception(1),enable counter(0). */
	STK->RVR = reload - 1;  /* Subtract one because this is multi-shot. */
	STK->CVR = 0;
}

void _stk_handler()
{
	++s_ticks;
}

int timer_expired(uint32_t *t, uint32_t prd, uint32_t now)
{
	if (now + prd < *t) *t = 0;  /* Check if time wrapped. */
	if (*t == 0) *t = now + prd;
	if (*t > now) return 0;
	*t = (now - *t) > prd ? now + prd : *t + prd;  /* Adjust the timer to correct for lateness, or adjust the timer to the next period. This code is genius beyond my understanding. */
	return 1;
}

/*
From datasheet:

 Configure the corresponding mask bit in the EXTI_IMR register.

 Configure the trigger selection bits of the interrupt line (EXTI_RTSR and EXTI_FTSR)

 Configure the enable and mask bits that control the NVIC IRQ channel mapped to the EXTI so that an interrupt coming from one of the EXTI line can be correctly acknowledged.
*/
void exti_pin_init(uint16_t pin, uint8_t rising, uint8_t priority, func_ptr handler)
{
	/* TODO: disable/clear bits */
	RCC->APB2ENR |= BIT(0);
	uint8_t n = PIN_NUM(pin);

	EXTI->IMR |= BIT(n);  /* Unmask line. */
	volatile uint32_t edge_selection = (EXTI->FTSR - (0x04 * rising));  /* Rising/falling edge. */
	edge_selection |= BIT(n);

	SYSCFG->EXTICR[n / 4] |= PIN_BANK(pin) << ((n - (4 * (n / 4))) * 4);  /* Set source input for respective EXTI line. */
	
	uint8_t int_pos = n < 2 ? 12 : n < 4 ? 13 : 14;
	NVIC->ISER |= BIT(int_pos);  /* Enable line interrupts for EXTI section(1:0, 3:2, 15:4). */
	NVIC->IPR[int_pos / 4] |= priority << (int_pos % 4);
	
	line_funcs[n] = handler;
}

void exti_simple_init(func_ptr handler)
{
	RCC->APB2ENR |= BIT(0);

	EXTI->IMR |= BIT(0);
	EXTI->RTSR |= BIT(0);
	SYSCFG->EXTICR[0] |= 0;
	
	NVIC->ISER |= BIT(12);
	NVIC->IPR[3] |= 0;
	
	line_funcs[0] = handler;
}


void _exti_line_handler()
{
	line_funcs[0]();
	uint16_t gpio_lines = BIT_LAST(EXTI->PR, 16);
	for (int i = 0; i < 16; ++i) {
		if (gpio_lines & BIT(i)) {
			line_funcs[i]();  /* Execute appropriate functions. */
			EXTI->PR |= BIT(i);
		}
	}
}
