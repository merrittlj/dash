/*
 * filename:	hal.h
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	hardware abstraction library
 */

#ifndef HAL_H
#define HAL_H
#include <stdint.h>
#include <stddef.h>

#include "stm32f072xb.h"
#include "common.h"


/* TODO: find a better method to get the system clock... */
#define FREQ 8000000
#define MS_MUL 5  /* The amount of times per millisecond we want SysTick to run(ex: 2 = SysTick runs every 0.5 ms). */
#define s_ticks_ms (s_ticks / MS_MUL)

/* GPIOA is 0x48000000, all subsequent banks are 0x400(1kb) apart. */
#define GPIO(bank) ((GPIO_TypeDef *) (GPIOA_BASE + (0x400U * (bank))))
#define BANK(letter) ((uint8_t)((letter) - 'A'))

#define BIT(x) (1UL << (x))
#define BIT_LAST(x, y) (x & ((1UL << y) - 1))  /* Last y bits. */
#define BITS_SET(x) ((BIT(x) - 1))
#define BIT_FIELD_CLEAR(x, size) ((uint32_t)~(BITS_SET(size) << (x * size)))
#define BIT_FIELD_SET(x, size, val) ((val & BITS_SET(size)) << (x * size))

/* uint16_t(2 bytes): upper byte = bank #, lower byte = pin # */
#define PIN(bank, num) ((uint16_t)((uint16_t)((uint16_t)(bank) - (uint16_t)('A')) << 8UL) | (uint16_t)(num))
#define PIN_NUM(pin) ((uint8_t)(BIT_LAST(pin, 8)))
#define PIN_BANK(pin) ((uint8_t)((pin) >> 8))

/* GPIO input/output modes. */
enum {GPIO_PULL_NONE, GPIO_PULL_UP, GPIO_PULL_DOWN};
enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG};
enum {GPIO_OUTPUT_CLEAR, GPIO_OUTPUT_SET};
enum {GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH = 3};

/* Enable/disable RCC. */
enum {RCC_PORT_DISABLE, RCC_PORT_ENABLE};

extern void gpio_set_pull(uint16_t pin, uint8_t mode);
extern void gpio_set_mode(uint16_t pin, uint8_t mode);
extern void gpio_write(uint16_t pin, uint8_t mode);
extern uint8_t gpio_read(uint16_t pin);
extern void gpio_set_af(uint16_t pin, uint8_t af);
extern void gpio_set_speed(uint16_t pin, uint8_t speed);

/* Enable/disable the clock on a GPIO port. */
extern void rcc_port_set(uint8_t bank, uint8_t mode);

extern volatile uint32_t s_ticks;

extern void disable_timer(uint32_t *prd);
extern void enable_timer(uint32_t *t, uint32_t *prd, uint32_t new_prd, uint32_t now);
extern int timer_expired(uint32_t *t, uint32_t prd, uint32_t now);
extern void hard_delay(uint32_t prd);  /* Hard delay, like Arduino's delay(). */

extern func_ptr line_funcs[32];  /* EXTI line functions. */
extern void exti_pin_init(uint16_t pin, uint8_t rising, uint8_t priority, func_ptr handler);


/* TX/RX pads are connected to USART2. */
extern void uart_init(USART_TypeDef *uart, uint32_t baud);
extern uint8_t uart_read_ready(USART_TypeDef *uart);
extern uint8_t uart_read_byte(USART_TypeDef *uart);
extern void uart_write_byte(USART_TypeDef *uart, uint8_t byte);
extern void uart_write_buf(USART_TypeDef *uart, char *buf, size_t len);

/* Functions referenced in CMSIS. */
extern void SysTick_Handler();

extern void EXTI_Common_IRQHandler();  /* Not defined in CMSIS, but common handler. */
/* All just call EXTI_Common_IRQHandler(). */
extern void EXTI0_1_IRQHandler();
extern void EXTI2_3_IRQHandler();
extern void EXTI4_15_IRQHandler();

extern void SystemInit(void);

extern void _init(void);  /* Please the linker beast. */

#endif
