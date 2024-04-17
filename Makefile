MAKE_DIR = $(PWD)
SRC_DIR	 = $(MAKE_DIR)/src

ROOT_DIR := $(SRC_DIR)/root
HAL_DIR  := $(SRC_DIR)/hal

INC_SRCH_PATH :=
INC_SRCH_PATH += -I$(ROOT_DIR)
INC_SRCH_PATH += -I$(HAL_DIR)

LIB_SRCH_PATH := 
LIB_SRCH_PATH += -L$(MAKE_DIR)/libs

LIBS := -lhal -lc -lgcc

# Toolchain
CC  := arm-none-eabi-gcc
LD  := arm-none-eabi-gcc
CPY := arm-none-eabi-objcopy

# Device-specific flags 
DFLAGS := -mcpu=cortex-m0 -mthumb -mfloat-abi=softfp -mfpu=auto
# Compiler flags
CFLAGS := -g3 -Os -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion -Wformat-truncation -Wpadded -Wconversion  -ffunction-sections -fdata-sections -fno-common  # -fno-short-enums # newlib doesn't work well with this
CFLAGS += $(DFLAGS)
CFLAGS += $(EXTRA_CFLAGS)
CFLAGS += $(INC_SRCH_PATH) $(LIB_SRCH_PATH)

export MAKE_DIR LIBS CC LD CPY CFLAGS


.DEFAULT_GOAL = all

all: build flash

build:
	@$(MAKE) -C $(HAL_DIR) -f hal.mk
	@$(MAKE) -C $(ROOT_DIR) -f root.mk

clean:
	@$(MAKE) -C $(HAL_DIR) -f hal.mk clean
	@$(MAKE) -C $(ROOT_DIR) -f root.mk clean

flash:
	@st-flash --reset write $(MAKE_DIR)/prog/firmware.bin 0x8000000

.PHONY: all build clean flash 
