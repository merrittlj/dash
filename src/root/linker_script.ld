ENTRY(Reset_Handler)	/* Defined in CMSIS. */

MEMORY
{
	/* memory (read/write/execute): ORIGIN = starting locatrion, LENGTH = memory size */
	FLASH (rx): ORIGIN = 0x08000000, LENGTH = 128K
	SRAM (rwx): ORIGIN = 0x20000000, LENGTH = 16K
}

_estack = ORIGIN(SRAM) + LENGTH(SRAM);  /* Initial stack address */

SECTIONS
{
	.isr_vector :
	{
		KEEP(*(.isr_vector))
	} >FLASH  /* ">" at the end of a section specifies where in the memory the section should be placed. */

	.text :
	{
		. = ALIGN(4);
	
		*(.text)
		*(.rodata)
	
		. = ALIGN(4);
	} >FLASH

	.data :
	{
		. = ALIGN(4);
		_sdata = .;
	
		*(.data)

		. = ALIGN(4);
		_edata = .;
	} >SRAM AT> FLASH  /* SRAM is relocation address(where the section will be when the code is executed), FLASH is load address(where the section is loaded from to the target). */
	_sidata = LOADADDR(.data);  /* Set to .data's load address(FLASH). */

	.bss :
	{
		. = ALIGN(4);
		_sbss = .;
	
		*(.bss)
	
		. = ALIGN(4);
		_ebss = .;
	} >SRAM
	
	. = ALIGN(8);
	_end = .;
}
