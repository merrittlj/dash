MAKE_DIR = $(PWD)
SRC_DIR	 = $(MAKE_DIR)/src

INCLUDE_DIR := $(SRC_DIR)/include
CMSIS_CORE_DIR := $(INCLUDE_DIR)/cmsis_core
CMSIS_FX_DIR := $(INCLUDE_DIR)/cmsis_f0
TFP_DIR := $(INCLUDE_DIR)/tinyprintf
ROOT_DIR    := $(SRC_DIR)/root
HAL_DIR     := $(SRC_DIR)/hal
PROG_DIR    := $(SRC_DIR)/prog
MAG_DIR    := $(SRC_DIR)/mag
STATE_DIR   := $(SRC_DIR)/state
SEG_DIR    := $(SRC_DIR)/seg
EMBMATH_DIR    := $(SRC_DIR)/embmath
LOG_DIR   := $(SRC_DIR)/log

INC_SRCH_PATH :=
INC_SRCH_PATH += -I$(INCLUDE_DIR)
INC_SRCH_PATH += -I$(CMSIS_CORE_DIR)/CMSIS/Core/Include
INC_SRCH_PATH += -I$(CMSIS_FX_DIR)/Include
INC_SRCH_PATH += -I$(TFP_DIR)
INC_SRCH_PATH += -I$(ROOT_DIR)
INC_SRCH_PATH += -I$(HAL_DIR)
INC_SRCH_PATH += -I$(PROG_DIR)
INC_SRCH_PATH += -I$(MAG_DIR)
INC_SRCH_PATH += -I$(STATE_DIR)
INC_SRCH_PATH += -I$(SEG_DIR)
INC_SRCH_PATH += -I$(EMBMATH_DIR)
INC_SRCH_PATH += -I$(LOG_DIR)

LIB_SRCH_PATH := 
LIB_SRCH_PATH += -L$(MAKE_DIR)/libs

LIBS := -lhal -lprog -lmag -lstate -lseg -lembmath -llog -lc -lgcc

# Toolchain
CC  := arm-none-eabi-gcc
LD  := arm-none-eabi-gcc
CPY := arm-none-eabi-objcopy

# Device-specific flags 
DFLAGS := -mcpu=cortex-m0 -mthumb -mfloat-abi=softfp -mfpu=auto
# Compiler flags
CFLAGS := -O0 -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion -Wformat-truncation -Wpadded -Wconversion  -ffunction-sections -fdata-sections -fno-common  # -fno-short-enums # newlib doesn't work well with this
CFLAGS += $(DFLAGS)
CFLAGS += $(EXTRA_CFLAGS)
CFLAGS += $(INC_SRCH_PATH) $(LIB_SRCH_PATH)
CFLAGS += -D __ARM_FP=0 -D __ARM_FEATURE_SAT=0 -D __ARM_FEATURE_LDREX=0 -D __ARM_FEATURE_DSP=0
CFLAGS += -g3 -ggdb3

export MAKE_DIR CMSIS_CORE_DIR CMSIS_FX_DIR TFP_DIR LIBS CC LD CPY CFLAGS


.DEFAULT_GOAL = all

all: build flash

dep: $(CMSIS_CORE_DIR) $(CMSIS_FX_DIR)

$(CMSIS_CORE_DIR):
	@git clone --depth 1 -b v6.0.0 https://github.com/ARM-software/CMSIS_6 $@
$(CMSIS_FX_DIR):
	@git clone --depth 1 -b v2.3.7 https://github.com/STMicroelectronics/cmsis_device_f0 $@

build: dep
	@$(MAKE) -C $(HAL_DIR) -f hal.mk
	@$(MAKE) -C $(PROG_DIR) -f prog.mk
	@$(MAKE) -C $(MAG_DIR) -f mag.mk
	@$(MAKE) -C $(STATE_DIR) -f state.mk
	@$(MAKE) -C $(SEG_DIR) -f seg.mk
	@$(MAKE) -C $(EMBMATH_DIR) -f embmath.mk
	@$(MAKE) -C $(LOG_DIR) -f log.mk
	@$(MAKE) -C $(ROOT_DIR) -f root.mk

full_clean: clean dep_clean

clean:
	@$(MAKE) -C $(HAL_DIR) -f hal.mk clean
	@$(MAKE) -C $(PROG_DIR) -f prog.mk clean
	@$(MAKE) -C $(MAG_DIR) -f mag.mk clean
	@$(MAKE) -C $(STATE_DIR) -f state.mk clean
	@$(MAKE) -C $(SEG_DIR) -f seg.mk clean
	@$(MAKE) -C $(LOG_DIR) -f log.mk clean
	@$(MAKE) -C $(ROOT_DIR) -f root.mk clean

dep_clean:
	@$(RM) -rf $(INCLUDE_DIR)/cmsis_core
	@$(RM) -rf $(INCLUDE_DIR)/cmsis_f0

rebuild: clean build

flash:
	@openocd -f interface/stlink.cfg -f board/stm32f0discovery.cfg -c "program bin/firmware.elf verify reset exit"

debug:
	@openocd -f interface/stlink.cfg -f board/stm32f0discovery.cfg -c "init" &
	@arm-none-eabi-gdb bin/firmware.elf -x gdbconf

test:
	@act --job act-linux

serial:
	@picocom -b 9600 -f n -y n -d 8 -p 1 -c /dev/ttyUSB0

.PHONY: all dep cmsis_core cmsis_f0 build clean dep_clean flash debug test serial
