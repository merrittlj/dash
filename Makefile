MAKE_DIR = $(PWD)
SRC_DIR	 = $(MAKE_DIR)/src

INCLUDE_DIR := $(SRC_DIR)/include
ROOT_DIR    := $(SRC_DIR)/root
HAL_DIR     := $(SRC_DIR)/hal
PROG_DIR    := $(SRC_DIR)/prog
STATE_DIR   := $(SRC_DIR)/state

INC_SRCH_PATH :=
INC_SRCH_PATH += -I$(INCLUDE_DIR)
INC_SRCH_PATH += -I$(ROOT_DIR)
INC_SRCH_PATH += -I$(HAL_DIR)
INC_SRCH_PATH += -I$(PROG_DIR)
INC_SRCH_PATH += -I$(STATE_DIR)

LIB_SRCH_PATH := 
LIB_SRCH_PATH += -L$(MAKE_DIR)/libs

LIBS := -lhal -lprog -lstate -lc -lgcc

# Toolchain
CC  := arm-none-eabi-gcc
LD  := arm-none-eabi-gcc
CPY := arm-none-eabi-objcopy

# Device-specific flags 
DFLAGS := -mcpu=cortex-m0 -mthumb -mfloat-abi=softfp -mfpu=auto
# Compiler flags
CFLAGS := -Os -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion -Wformat-truncation -Wpadded -Wconversion  -ffunction-sections -fdata-sections -fno-common  # -fno-short-enums # newlib doesn't work well with this
CFLAGS += $(DFLAGS)
CFLAGS += $(EXTRA_CFLAGS)
CFLAGS += $(INC_SRCH_PATH) $(LIB_SRCH_PATH)
CFLAGS += -g3 -ggdb3


.DEFAULT_GOAL = all

all: build flash

build:
	@$(MAKE) -C $(HAL_DIR) -f hal.mk
	@$(MAKE) -C $(PROG_DIR) -f prog.mk
	@$(MAKE) -C $(STATE_DIR) -f state.mk
	
	@$(MAKE) -C $(ROOT_DIR) -f root.mk

clean:
	@$(MAKE) -C $(HAL_DIR) -f hal.mk clean
	@$(MAKE) -C $(PROG_DIR) -f prog.mk clean
	@$(MAKE) -C $(STATE_DIR) -f state.mk clean

	@$(MAKE) -C $(ROOT_DIR) -f root.mk clean

flash:
	@openocd -f interface/stlink-v2.cfg -f board/stm32f0discovery.cfg -c "program bin/firmware.elf verify reset exit"

debug:
	@openocd -f interface/stlink-v2.cfg -f board/stm32f0discovery.cfg -c "init" &
	@arm-unknown-eabi-gdb bin/firmware.elf -x gdbconf

.PHONY: all build clean flash debug
