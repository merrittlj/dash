TARGET = $(MAKE_DIR)/bin/firmware.elf
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

LSCRIPT := ./linker_script.ld

LDFLAGS ?= -T $(LSCRIPT) -nostartfiles -nostdlib --specs nano.specs -Wl,--gc-sections -Wl,-Map=$(PROG).map

OFLAGS  ?= -O binary


$(TARGET): $(SRCS)
	@mkdir -p $(MAKE_DIR)/bin
	@$(LD) $^ $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@echo "Generate program $(notdir $(TARGET)) from $^"

clean:
	@$(RM) -f $(OBJS) $(TARGET)
	@$(RM) -f *.expand
	@$(RM) -rf $(MAKE_DIR)/bin $(MAKE_DIR)/libs
	@echo "Remove objects: $(OBJS)"
	@echo "Remove libraries: $(notdir $(TARGET))"

.PHONY: clean
