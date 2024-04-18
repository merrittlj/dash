/*
 * filename:	hal.c
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	implementation for hal.h
 */

#include <stdint.h>

#include "hal.h"


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

