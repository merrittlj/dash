/*
 * filename:	hal.h
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	hardware abstraction library
 */

#ifndef HAL_H
#define HAL_H
#include <stdint.h>

#include "common.h"


struct gpio {
	volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};

/* GPIOA is 0x48000000, all subsequent banks are 0x400(1kb) apart. */
#define GPIO(bank) ((struct gpio *) (0x48000000 + (0x400 * (bank))))
#define BANK(letter) ((uint8_t)((letter) - 'A'))

#define BIT(x) (1UL << (x))
#define BIT_LAST(x, y) (x & ((1 << y) - 1))  /* Last y bits. */

/* uint16_t(2 bytes): upper byte = bank #, lower byte = pin # */
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))  /* ex: PIN('C', 7) */
#define PIN_NUM(pin) ((uint8_t)((pin) & 255))
#define PIN_BANK(pin) ((uint8_t)((pin) >> 8))

/* GPIO input/output modes. */
enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_ALT, GPIO_MODE_ANALOG};
enum {GPIO_OUTPUT_CLEAR, GPIO_OUTPUT_SET};

struct rcc {
	volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, BDCR, CSR, AHBRSTR, CFGR2, CFGR3, CR2;
};

#define RCC ((struct rcc *) 0x40021000)

/* Enable/disable RCC. */
enum {RCC_PORT_DISABLE, RCC_PORT_ENABLE};

extern void gpio_set_mode(uint16_t pin, uint8_t mode);
extern void gpio_write_output(uint16_t pin, uint8_t mode);
extern uint8_t gpio_read_input(uint16_t pin);
extern void rcc_port_enable(uint8_t bank, uint8_t mode);

/* SysTick */
struct stk {
	volatile uint32_t CSR, RVR, CVR, CALIB;
};

#define STK ((struct stk *) 0xE000E010)
extern volatile uint32_t s_ticks;

extern void stk_init(uint32_t reload);
extern void _stk_handler();
extern int timer_expired(uint32_t *t, uint32_t prd, uint32_t now);

struct nvic {
	volatile uint32_t ISER;
	uint32_t reserved0[31];
	volatile uint32_t ICER;
	uint32_t reserved1[31];
	volatile uint32_t ISPR;
	uint32_t reserved2[31];
	volatile uint32_t ICPR;
	uint32_t reserved3[95];
	volatile uint32_t IPR[8];
};

#define NVIC ((struct nvic *) 0x0E000E100)

struct syscfg {
	volatile uint32_t CFGR1, reserved, EXTICR[4], CFGR2;
};

#define SYSCFG ((struct syscfg *) 0x40010000)

struct exti {
	volatile uint32_t IMR, EMR, RTSR, FTSR, SWIER, PR;
};

#define EXTI ((struct exti *) 0x40010400)

extern func_ptr line_funcs[32];  /* EXTI line functions. */
extern void exti_pin_init(uint16_t pin, uint8_t rising, uint8_t priority, func_ptr handler);
extern void exti_simple_init(func_ptr handler);

extern void _exti_line_handler();

#endif
