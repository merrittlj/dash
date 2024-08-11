/*
 * filename:	seg.c
 * date:	04.28.24
 * author:	Lucas Merritt/merrittlj
 * description:	Implementation for seg.h
 */
#include <stdint.h>

#include "seg.h"
#include "hal.h"
#include "embmath.h"


static uint16_t _display_num;
static uint8_t _decimal_loc;
static uint8_t _num_index;
static uint8_t _subzero;

static uint16_t _ser;
static uint16_t _rclk;
static uint16_t _srclk;

static uint16_t _displays[4];

void seg_pins(uint16_t ser, uint16_t rclk, uint16_t srclk, uint16_t dp1, uint16_t dp2, uint16_t dp3, uint16_t dp4)
{
	_ser = ser;
	_rclk = rclk;
	_srclk = srclk;
	_displays[0] = dp1;
	_displays[1] = dp2;
	_displays[2] = dp3;
	_displays[3] = dp4;
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

void seg_new_manual(uint16_t num, uint8_t decimal)
{
	set_display_num(num);
	set_decimal_loc(decimal);
	set_num_index(0);
	_subzero = 0;
}

void seg_new(double num)
{
	uint8_t decimal_places = 0;
	double div = num;
	for (; div > 1; div /= 10, ++decimal_places) ;

	_subzero = 0;
	if (num < 1 && num >= 0.001) _subzero = 1;

	uint16_t x = (uint16_t)(num * ipow(10, 4 - decimal_places));
	if (!_subzero) set_display_num(x);
	if (_subzero) set_display_num(x / 10);

	set_decimal_loc(decimal_places % 4);
	set_num_index(0);
}

void __attribute__((optimize("O0"))) set_display_num(uint16_t num)
{
	_display_num = num;
}

uint16_t __attribute__((optimize("O0"))) get_display_num()
{
	return _display_num;
}

void __attribute__((optimize("O0"))) set_decimal_loc(uint8_t decimal)
{
	_decimal_loc = decimal;
}

uint8_t __attribute__((optimize("O0"))) get_decimal_loc()
{
	return _decimal_loc;
}

void __attribute__((optimize("O0"))) set_num_index(uint8_t index)
{
	_num_index = index;
}

uint8_t __attribute__((optimize("O0"))) get_num_index()
{
	return _num_index;
}

uint8_t current_digit()
{
	return (uint8_t)((_display_num / (uint16_t)(ipow(10, 3 - _num_index))) % 10);
}

void seg_display_next()
{
	seg_clear_output();
	if (_num_index > 0) gpio_write(_displays[_num_index - 1], GPIO_OUTPUT_CLEAR);
	if (_num_index == 0) gpio_write(_displays[3], GPIO_OUTPUT_CLEAR);
	/* This block displays the digit, skip to not display this digit */
	uint8_t skip = (current_digit() == 0 && (uint16_t)(ipow(10, 4 - _num_index)) > _display_num) && ((_display_num != 0) || (_num_index != 3)) && !_subzero;  /* Hide leading 0's */
	if (!skip) {
		gpio_write(_displays[_num_index], GPIO_OUTPUT_SET);
		uint8_t decimal_enabled = ((_num_index == (_decimal_loc - 1)) && _decimal_loc <= 4) || (_subzero && _num_index == 0);
		seg_display_digit(current_digit(), decimal_enabled);
	}

	++_num_index;
	_num_index = _num_index % 4;
}

