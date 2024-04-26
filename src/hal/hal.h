/*
 * filename:	hal.h
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	hardware abstraction library
 */

#ifndef HAL_H
#define HAL_H
#include <stdint.h>

#include "stm32f072xb.h"
#include "common.h"


/* GPIOA is 0x48000000, all subsequent banks are 0x400(1kb) apart. */
#define GPIO(bank) ((GPIO_TypeDef *) (GPIOA_BASE + (0x400U * (bank))))
#define BANK(letter) ((uint8_t)((letter) - 'A'))

#define BIT(x) (1UL << (x))
#define BIT_LAST(x, y) (x & ((1UL << y) - 1))  /* Last y bits. */

/* uint16_t(2 bytes): upper byte = bank #, lower byte = pin # */
//#define PIN(bank, num) ((uint16_t)((uint16_t)((uint16_t)(bank) - (uint16_t)('A')) << 8UL) | (uint16_t)(num))
#define PIN(bank, num) ((uint16_t)(((uint16_t)((unsigned char)(bank) - (unsigned char)('A'))) << 8) | (uint16_t)(num))
#define PIN_NUM(pin) ((uint8_t)(BIT_LAST(pin, 8)))
#define PIN_BANK(pin) ((uint8_t)((pin) >> 8))

/* GPIO input/output modes. */
enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_ALT, GPIO_MODE_ANALOG};
enum {GPIO_OUTPUT_CLEAR, GPIO_OUTPUT_SET};

/* Enable/disable RCC. */
enum {RCC_PORT_DISABLE, RCC_PORT_ENABLE};

extern void gpio_set_mode(uint16_t pin, uint8_t mode);
extern void gpio_write(uint16_t pin, uint8_t mode);
extern uint8_t gpio_read(uint16_t pin);
/* Enable/disable the clock on a GPIO port. */
extern void rcc_port_set(uint8_t bank, uint8_t mode);

extern volatile uint32_t s_ticks;

extern int timer_expired(uint32_t *t, uint32_t prd, uint32_t now);

extern func_ptr line_funcs[32];  /* EXTI line functions. */
extern void exti_pin_init(uint16_t pin, uint8_t rising, uint8_t priority, func_ptr handler);
extern void exti_simple_init(func_ptr handler);


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
