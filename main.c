/*
 * filename:	main.c
 * date:	03.29.24
 * author:	Lucas Merritt/merrittlj
 * description:	Program core.
 */

#include <stdint.h>


typedef void (*func_ptr)(void);

struct gpio {
	volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};

/* GPIOA is 0x48000000, all subsequent banks are 0x400(1kb) apart. */
#define GPIO(bank) ((struct gpio *) (0x48000000 + (0x400 * (bank))))
#define BANK(letter) ((uint8_t)((letter) - 'A'))

#define BIT(x) (1UL << (x))

/* uint16_t(2 bytes): upper byte = bank #, lower byte = pin # */
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))  /* ex: PIN('C', 7) */
#define PIN_NUM(pin) ((uint8_t)((pin) & 255))
#define PIN_BANK(pin) ((uint8_t)((pin) >> 8))

enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_ALT, GPIO_MODE_ANALOG};
enum {GPIO_OUTPUT_CLEAR, GPIO_OUTPUT_SET};

struct rcc {
	volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, BDCR, CSR, AHBRSTR, CFGR2, CFGR3, CR2;
};

#define RCC ((struct rcc *) 0x40021000)

enum {RCC_PORT_DISABLE, RCC_PORT_ENABLE};

enum {LED_COLOR_RED = 6, LED_COLOR_BLUE, LED_COLOR_ORANGE, LED_COLOR_GREEN};

struct state {
	func_ptr action;  /* What action to take when transitioning in. */
	func_ptr transition;  /* What action to take when transitioning out. */
	uint8_t next;  /* Next state. */

	char _padding[3];
};

/* The FSM array does not directly depend on what indexes the states are at. */
enum {STATE_MAX_SPEED, STATE_TRIP_DISTANCE, STATE_TOTAL_DISTANCE, NUM_STATES};


static inline void gpio_set_mode(uint16_t pin, uint8_t mode)
{
	struct gpio *b = GPIO(PIN_BANK(pin));
	uint8_t n = PIN_NUM(pin);

	// 2 bits for each pin so we multiply the pin # by 2.
	b->MODER &= (uint32_t)~(3 << (n * 2));  /* Clear pin mode. */
	b->MODER |= (mode & 3) << (n * 2);  /* Set pin mode("& 3" ensures bounds). */
}

static inline void gpio_write_output(uint16_t pin, uint8_t mode)
{
	GPIO(PIN_BANK(pin))->BSRR |= BIT(((mode ^ 1) * 16) + PIN_NUM(pin));
}

static inline uint8_t gpio_read_input(uint16_t pin)
{
	return (GPIO(PIN_BANK(pin))->IDR >> PIN_NUM(pin)) & 1;
}

static inline void rcc_port_enable(uint8_t bank, uint8_t mode)
{
	int n = (17 + bank);
	RCC->AHBENR = (RCC->AHBENR & ~BIT(n)) | (mode << n);
}

void spin(volatile uint32_t x)
{
	while (x--) (void) 0;
}

static inline void write_builtin_led(uint8_t color, uint8_t mode)
{
	rcc_port_enable(BANK('C'), RCC_PORT_ENABLE);
	gpio_set_mode(PIN('C', color), GPIO_MODE_OUTPUT);
	gpio_write_output(PIN('C', color), mode);
}

void max_speed_action()
{
	write_builtin_led(LED_COLOR_RED, GPIO_OUTPUT_SET);
}

void max_speed_transition()
{
	write_builtin_led(LED_COLOR_RED, GPIO_OUTPUT_CLEAR);
}

void trip_distance_action()
{
	write_builtin_led(LED_COLOR_GREEN, GPIO_OUTPUT_SET);
}

void trip_distance_transition()
{
	write_builtin_led(LED_COLOR_GREEN, GPIO_OUTPUT_CLEAR);
}

void total_distance_action()
{
	write_builtin_led(LED_COLOR_BLUE, GPIO_OUTPUT_SET);
}

void total_distance_transition()
{
	write_builtin_led(LED_COLOR_BLUE, GPIO_OUTPUT_CLEAR);
}

int main()
{
	const struct state FSM[NUM_STATES] = {
		[STATE_MAX_SPEED] = {
			max_speed_action,
			max_speed_transition,
			.next = STATE_TRIP_DISTANCE
		},

		[STATE_TRIP_DISTANCE] = {
			trip_distance_action,
			trip_distance_transition,
			.next = STATE_TOTAL_DISTANCE
		},

		[STATE_TOTAL_DISTANCE] = {
			total_distance_action,
			total_distance_transition,
			.next = STATE_MAX_SPEED
		}
	};

	struct state current_state = FSM[STATE_MAX_SPEED];

	uint16_t usrbtn = PIN('A', 0);

	rcc_port_enable(PIN_BANK(usrbtn), RCC_PORT_ENABLE);
	gpio_set_mode(usrbtn, GPIO_MODE_INPUT);
	
	current_state.action();
	for (;;) {
		if (gpio_read_input(usrbtn)) {
			current_state.transition();
			current_state = FSM[current_state.next];
			current_state.action();

			/* Wait until button is not pressed. */
			while (gpio_read_input(usrbtn)) (void) 0;
			spin(59999);  /* Handle button debounce. */
		}
	}

	return 0;
}
