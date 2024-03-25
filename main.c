#include <stdint.h>


struct gpio {
	volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};

/* GPIOA is 0x48000000, all subsequent banks are 0x400(1kb) apart. */
#define GPIO(bank) ((struct gpio *) (0x48000000 + (0x400 * (bank))))

/* uint16_t(2 bytes): upper byte = bank #, lower byte = pin # */
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))  /* ex: PIN('C', 7) */
#define PIN_NUM(pin) (pin & 255)
#define PIN_BANK(pin) (pin >> 8)

enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_ALT, GPIO_MODE_ANALOG};

static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
	struct gpio *b = GPIO(PIN_BANK(pin));
	uint8_t n = PIN_NUM(pin);

	// 2 bits for each pin so we multiply the pin # by 2.
	b->MODER &= ~(3 << (n * 2));  /* Clear pin mode. */
	b->MODER |= (mode & 3) << (n * 2);  /* Set pin mode("& 3" ensures bounds). */
}

int main()
{
	return 0;
}
