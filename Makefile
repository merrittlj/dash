default:
	arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
	st-flash write firmware.bin 0x8000000
