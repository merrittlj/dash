/*
 * filename:	hal.c
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	implementation for hal.h
 */

#include <stdint.h>

#include "stm32f072xb.h"
#include "common.h"

#include "hal.h"


volatile uint32_t s_ticks = 0;

func_ptr line_funcs[32];

void gpio_set_pull(uint16_t pin, uint8_t mode)
{
	GPIO_TypeDef *b = GPIO(PIN_BANK(pin));
	uint8_t n = PIN_NUM(pin);
	b->PUPDR &= BIT_FIELD_CLEAR(n, 2);
	b->PUPDR |= BIT_FIELD_SET(n, 2, mode);
}

void gpio_set_mode(uint16_t pin, uint8_t mode)
{
	GPIO_TypeDef *b = GPIO(PIN_BANK(pin));
	uint8_t n = PIN_NUM(pin);

	rcc_port_set(PIN_BANK(pin), RCC_PORT_ENABLE);
	if (mode == GPIO_MODE_INPUT) gpio_set_pull(pin, GPIO_PULL_UP);

	b->MODER &= BIT_FIELD_CLEAR(n, 2);
	b->MODER |= BIT_FIELD_SET(n, 2, mode);
}

void gpio_write(uint16_t pin, uint8_t mode)
{
	GPIO(PIN_BANK(pin))->BSRR |= BIT(((mode ^ 1) * 16) + PIN_NUM(pin));
}

uint8_t gpio_read(uint16_t pin)
{
	return (GPIO(PIN_BANK(pin))->IDR >> PIN_NUM(pin)) & 1;
}

void gpio_set_af(uint16_t pin, uint8_t af)
{
	GPIO_TypeDef *gpio = GPIO(PIN_BANK(pin));
	uint8_t n = (PIN_NUM(pin)) - (8 * (n >> 3));
	gpio->AFR[n >> 3] &= BIT_FIELD_CLEAR(n, 4);
	gpio->AFR[n >> 3] |= BIT_FIELD_SET(n, 4, af);
}

void rcc_port_set(uint8_t bank, uint8_t mode)
{
	int n = (17 + bank);
	RCC->AHBENR = (RCC->AHBENR & ~BIT(n)) | (uint32_t)(mode << n);  /* Clear, then set bit. */
}

void disable_timer(uint32_t *prd)
{
	*prd = 0;
}

void enable_timer(uint32_t *t, uint32_t *prd, uint32_t new_prd, uint32_t now)
{
	*prd = new_prd;
	*t = now + *prd;
}

int timer_expired(uint32_t *t, uint32_t prd, uint32_t now)
{
	if (prd == 0) return 0;  /* Timer disabled. */
	if (now + prd < *t) *t = 0;  /* Check if time wrapped. */
	if (*t == 0) *t = now + prd;
	if (*t > now) return 0;
	*t = (now - *t) > prd ? now + prd : *t + prd;  /* Adjust the timer to correct for lateness, or adjust the timer to the next period. */
	return 1;
}

void hard_delay(uint32_t prd)
{
	const uint32_t t = s_ticks + prd;
	while (t > s_ticks) (void) 0;
}

/*
From datasheet:

 Configure the corresponding mask bit in the EXTI_IMR register.

 Configure the trigger selection bits of the interrupt line (EXTI_RTSR and EXTI_FTSR)

 Configure the enable and mask bits that control the NVIC IRQ channel mapped to the EXTI so that an interrupt coming from one of the EXTI line can be correctly acknowledged.
*/
void exti_pin_init(uint16_t pin, uint8_t rising, uint8_t priority, func_ptr handler)
{
	uint8_t n = PIN_NUM(pin);

	EXTI->IMR |= BIT(n);  /* Unmask line. */
	volatile uint32_t *edge_selection = (&EXTI->FTSR - rising);  /* Rising/falling edge. */
	*edge_selection |= BIT(n);

	SYSCFG->EXTICR[n / 4] |= (uint32_t)(PIN_BANK(pin) << ((n - (4 * (n / 4))) * 4));  /* Set source input for respective EXTI line. */
	
	uint8_t irq_pos = n < 2 ? EXTI0_1_IRQn : n < 4 ? EXTI2_3_IRQn : EXTI4_15_IRQn;
	NVIC_EnableIRQ(irq_pos);
	NVIC_SetPriority(irq_pos, priority);
	
	line_funcs[n] = handler;
}

void uart_init(struct USART_TypeDef *uart, uint32_t baud)
{
	if (uart == USART1) RCC->APB2ENR |= BIT(14);
	if (uart == USART2) RCC->APB1ENR |= BIT(17);
	if (uart == USART3) RCC->APB1ENR |= BIT(18);
	
	uint16_t tx, rx;
	if (uart == USART1) tx = PIN('B', 6), rx = PIN('B', 7);
	if (uart == USART2) tx = PIN('D', 5), rx = PIN('D', 6);
	if (uart == USART3) tx = PIN('D', 8), rx = PIN('D', 9);

	gpio_set_mode(tx, GPIO_MODE_AF);
	gpio_set_mode(rx, GPIO_MODE_AF);
	gpio_set_af(tx, 0);
	gpio_set_af(rx, 0);

	uart->CR1 = 0;  /* Disable this line. */
	uart->BRR = FREQ / baud;
}

void SysTick_Handler()
{
	++s_ticks;
}

void EXTI_Common_IRQHandler()
{
	uint16_t gpio_lines = BIT_LAST(EXTI->PR, 16);
	for (int i = 0; i < 16; ++i) {
		if (gpio_lines & BIT(i)) {
			EXTI->PR &= BIT(i);
			line_funcs[i]();  /* Execute appropriate functions. */
		}
	}
}

void EXTI0_1_IRQHandler()
{
	EXTI_Common_IRQHandler();
}

void EXTI2_3_IRQHandler()
{
	EXTI_Common_IRQHandler();
}

void EXTI4_15_IRQHandler()
{
	EXTI_Common_IRQHandler();
}

void SystemInit(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	SysTick_Config(8000000 / 1000);
	NVIC_SetPriority (SysTick_IRQn, 0);
}

void _init() {;}
