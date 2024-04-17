PROG = $(MAKE_DIR)/prog/firmware.bin
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

LSCRIPT := ./linker_script.ld

LDFLAGS ?= -T $(LSCRIPT) -nostartfiles -nostdlib --specs nano.specs -Wl,--gc-sections -Wl,-Map=$(PROG).map

OFLAGS  ?= -O binary


$(PROG): $(SRCS)
	@mkdir -p $(MAKE_DIR)/prog
	@$(LD) $^ $(CFLAGS) $(LDFLAGS) $(LIBS) -o intermediate.elf
	@$(CPY) $(OFLAGS) intermediate.elf $@
	@echo "Generate program $(notdir $(PROG)) from $^"

clean:
	@$(RM) -f $(OBJS) $(PROG)
	@$(RM) -f *.expand
	@$(RM) -rf ../prog ../libs
	@echo "Remove objects: $(OBJS)"
	@echo "Remove libraries: $(notdir $(PROG))"

.PHONY: clean
