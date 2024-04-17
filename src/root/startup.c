/*
 * filename:	startup.c
 * date:	03.29.24
 * author:	Lucas Merritt/merrittlj
 * description:	Startup code, prepares memory and MCU.
 */

#include "main.h"

/* Reset handler/startup code. */
__attribute__((naked, noreturn))
void _reset_handler(void)
{
	extern long _sdata, _edata, _fdata, _sbss, _ebss;
	/* Copy flash .data to SRAM */
	for (long *src = &_fdata, *dst = &_sdata; dst < &_edata;) *dst++ = *src++;
	/* Zero-out .bss. */
	for (long *dst = &_sbss; dst < &_ebss;) *dst++ = 0;

	main();
	for (;;) (void) 0;
}

extern void _estack(void);  /* Defined in linker_script.ld */

/* 16 standard and 32 STM32 handlers. */
__attribute__((section(".isr_vector")))  /* Attribute places array in vector table. */
void (*const table[16 + 32])(void) = {
	_estack,  /* Initial stack address */
	_reset_handler  /* Boot function */
};
