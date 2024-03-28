CFLAGS ?= -g3 -Os -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion -Wformat-truncation -Wpadded -Wconversion  -ffunction-sections -fdata-sections -fno-common -fno-short-enums -I. -mcpu=cortex-m0 -mthumb -mfloat-abi=softfp -mfpu=auto $(EXTRA_CFLAGS)
LDFLAGS ?= -T linker_script.ld -nostartfiles -nostdlib --specs nano.specs -l c -l gcc -Wl,--gc-sections -Wl,-Map=$@.map
SOURCES = startup.c main.c


build: firmware.elf

clean:
	rm -rf firmware.*

firmware.elf: $(SOURCES)
	arm-none-eabi-gcc $< -o $@ $(CFLAGS) $(LDFLAGS)
	
firmware.bin: firmware.elf
	arm-none-eabi-objcopy -O binary $< $@

flash: firmware.bin
	st-flash --reset write $< 0x8000000
