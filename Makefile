# Resulting file
TARGET	:= firmware.bin
# Project paths
LIB	:= ./lib
INC	:= ./inc
ARC	:= ./arc
OBJ	:= ./obj

# Toolchain
CC	:= arm-none-eabi-gcc 
LD 	:= arm-none-eabi-gcc
CPY 	:= arm-none-eabi-objcopy 
# Linker script
LSCRIPT	:= linker_script.ld

# Device-specific flags 
DFLAGS	?= -mcpu=cortex-m0 -mthumb -mfloat-abi=softfp -mfpu=auto
# Compiler flags
CFLAGS	?= -g3 -Os -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion -Wformat-truncation -Wpadded -Wconversion  -ffunction-sections -fdata-sections -fno-common -fno-short-enums -I. $(DFLAGS) $(EXTRA_CFLAGS)
# Linker flags
LDFLAGS	?= -T $(LSCRIPT) -nostartfiles -nostdlib --specs nano.specs -l c -l gcc -Wl,--gc-sections -Wl,-Map=$@.map
# Object copy flags
OFLAGS	?= -O binary 

# Sources
SOURCES	:= $(shell find $(LIB) -type f -name *.c)
OBJECTS	:= $(patsubst $(LIB)/%.c, $(OBJ)/%.o, $(SOURCES))

.DEFAULT_GOAL := all


all: build program

build: $(OBJ)/$(TARGET)

clean:
	rm -rf $(OBJ)/*

$(OBJ)/firmware.elf: $(SOURCES)
	$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)
	
$(OBJ)/$(TARGET): $(OBJ)/firmware.elf
	$(CPY) $< $@

program: $(OBJ)/$(TARGET)
	st-flash --reset write $< 0x8000000
