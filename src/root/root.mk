TARGET = $(MAKE_DIR)/bin/firmware.elf
STARTUP = $(CMSIS_FX_DIR)/Source/Templates/gcc/startup_stm32f072xb.s
SRCS = $(filter-out old/*, $(wildcard *.c))
SRCS += $(STARTUP)
OBJS = $(filter-out $(STARTUP), $(patsubst %.c, %.o, $(SRCS)))

LSCRIPT := linker_script.ld

LDFLAGS ?= -T$(LSCRIPT) -nostartfiles -nostdlib --specs nano.specs -Wl,--gc-sections -Wl,-Map=$(TARGET).map

OFLAGS  ?= -O binary


$(TARGET): $(SRCS)
	@mkdir -p $(MAKE_DIR)/bin
	@$(LD) $^ $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@echo "Generate program $(notdir $(TARGET)) from $^"

$(SRCS): $(LSCRIPT)

clean:
	@$(RM) -f $(OBJS) $(TARGET) $(TARGET).map
	@$(RM) -f *.expand
	@$(RM) -rf $(MAKE_DIR)/bin $(MAKE_DIR)/libs
	@echo "Remove objects: $(OBJS)"
	@echo "Remove libraries: $(notdir $(TARGET))"

.PHONY: clean
