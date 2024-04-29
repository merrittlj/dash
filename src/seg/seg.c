/*
 * filename:	seg.c
 * date:	04.28.24
 * author:	Lucas Merritt/merrittlj
 * description:	Implementation for seg.h
 */
#include <stdint.h>
#include <math.h>

#include "seg.h"
#include "hal.h"


static uint16_t _display_num;
static uint8_t _decimal_loc;
static uint8_t _num_index;

static uint16_t _ser;
static uint16_t _rclk;
static uint16_t _srclk;

void seg_pins(uint16_t ser, uint16_t rclk, uint16_t srclk)
{
	_ser = ser;
	_rclk = rclk;
	_srclk = srclk;
}

void seg_update_output()
{
	gpio_write(_rclk, GPIO_OUTPUT_SET);
	gpio_write(_rclk, GPIO_OUTPUT_CLEAR);
}

void seg_send_bit(uint8_t bit)
{
	gpio_write(_ser, bit);
	gpio_write(_srclk, GPIO_OUTPUT_SET);
	gpio_write(_srclk, GPIO_OUTPUT_CLEAR);
	gpio_write(_ser, GPIO_OUTPUT_CLEAR);
}

void seg_clear_output()
{
	for (uint8_t i = 0; i < 8; ++i) seg_send_bit(0);
	seg_update_output();
}

void seg_display_digit(uint8_t digit, uint8_t decimal_enabled)
{
	/*
	 * Index correspondent to digit. Encodings are little-endian from A-G, with 0 = off and 1 = on.
	 * Ex: 0 is all segments but G on, 0x3F = 0b0111111, we send 0, then 1, then 1, etc.
	 */
	uint8_t decode_nums[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

	/* We iterate in reverse order through the bits (ex: 0 = 0, then 1, then 1, etc.) because of how the shift register is buffered. */
	seg_send_bit(decimal_enabled);
	for (int8_t i = 6; i >= 0; --i) seg_send_bit((decode_nums[digit] >> i) & 1);

	seg_update_output();
}

void seg_init(uint16_t num, uint8_t decimal)
{
	set_display_num(num);
	set_decimal_loc(decimal);
	set_num_index(0);
}

void set_display_num(uint16_t num)
{
	_display_num = num;
}

uint16_t get_display_num()
{
	return _display_num;
}

void set_decimal_loc(uint8_t decimal)
{
	_decimal_loc = decimal;
}

uint8_t get_decimal_loc()
{
	return _decimal_loc;
}

void set_num_index(uint8_t index)
{
	_num_index = index;
}

uint8_t get_num_index()
{
	return _num_index;
}

void seg_display_next()
{
	seg_clear_output();
	/* TODO: set display to _num_index */
	seg_display_digit((uint8_t)((_display_num / (uint16_t)(pow(10, 3 - _num_index))) % 10), _num_index == _decimal_loc);

	++_num_index;
	_num_index = _num_index % 4;
}

