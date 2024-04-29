/*
 * filename:	seg.h
 * date:	04.28.24
 * author:	Lucas Merritt/merrittlj
 * description:	Driver code for 4-digit seven segment display with 8-bit shift register.
 */

#ifndef SEG_H
#define SEG_H
#include <stdint.h>

#include "common.h"


extern void seg_pins(uint16_t ser, uint16_t rclk, uint16_t srclk);  /* Set pins to use. */

extern void seg_update_output();  /* Clock RCLK. */
extern void seg_send_bit(uint8_t bit);  /* Send a bit to the shift register. */
extern void seg_clear_output();
extern void seg_display_digit(uint8_t digit, uint8_t decimal_enabled);

extern void seg_init(uint16_t num, uint8_t decimal);

/* To display a multiple-digit number, we have to multiplex. Set a number to display and display each digit with a function. */
extern void set_display_num(uint16_t num);
extern uint16_t get_display_num();

/* Where the decimal is in _display_num, 0:3. */
extern void set_decimal_loc(uint8_t decimal);
extern uint8_t get_decimal_loc();

/* Current digit index in _display_num, 0:3. */
extern void set_num_index(uint8_t index);
extern uint8_t get_num_index();

extern void seg_display_next();  /* Display the next digit in the num. */

#endif
