ENTRY(reset_handler)	/* Enter in the reset_handler function(handles init.). */

MEMORY
{
	/* memory (read/write/execute): ORIGIN = starting location, LENGTH = memory size */
	FLASH (rx): ORIGIN = 0x08000000, LENGTH = 128K
	SRAM (rwx): ORIGIN = 0x20000000, LENGTH = 16K
}

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
		_etext = .;
	} >FLASH

	.data :
	{
		. = ALIGN(4);
		_sdata = .;
	
		*(.data)

		. = ALIGN(4);
		_edata = .;
	} >SRAM AT> FLASH  /* SRAM is relocation address(where the section will be when the code is executed), FLASH is load address(where the section is loaded from to the target). */

	.bss :
	{
		. = ALIGN(4);
		_sbss = .;
	
		*(.bss)
	
		. = ALIGN(4);
		_ebss = .;
	} >SRAM
}
