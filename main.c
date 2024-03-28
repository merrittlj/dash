#include <stdint.h>


struct gpio {
	volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};

/* GPIOA is 0x48000000, all subsequent banks are 0x400(1kb) apart. */
#define GPIO(bank) ((struct gpio *) (0x48000000 + (0x400 * (bank))))

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

static inline void rcc_port_enable(uint8_t port, uint8_t mode)
{
	int n = (17 + port);
	RCC->AHBENR = (RCC->AHBENR & ~BIT(n)) | (mode << n);
}

void spin(volatile uint32_t x)
{
	while (x--) (void) 0;
}

int main()
{
	uint16_t led = PIN('C', 6);
	uint16_t usrbtn = PIN('A', 0);

	rcc_port_enable(PIN_BANK(led), RCC_PORT_ENABLE);
	rcc_port_enable(PIN_BANK(usrbtn), RCC_PORT_ENABLE);
	gpio_set_mode(led, GPIO_MODE_OUTPUT);
	gpio_set_mode(usrbtn, GPIO_MODE_INPUT);

	for (;;) {
		while (gpio_read_input(usrbtn)) {
			gpio_write_output(led, GPIO_OUTPUT_SET);
			spin(999999);
			gpio_write_output(led, GPIO_OUTPUT_CLEAR);
			spin(999999);
			if (!gpio_read_input(usrbtn)) {
				led++;
				if (PIN_NUM(led) > 9) led = ((PIN_BANK(led) << 8) | 6);
				gpio_set_mode(led, GPIO_MODE_OUTPUT);
			}
		}
	}

	return 0;
}
