/*
 * filename:	hal.c
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	implementation for hal.h
 */

#include <stdint.h>

#include "hal.h"


volatile uint32_t s_ticks = 0;

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
