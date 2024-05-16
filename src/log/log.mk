LIB = $(MAKE_DIR)/libs/liblog.a
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))
CFLAGS += -Wno-sign-conversion -Wno-padded -Wno-conversion -Wno-implicit-fallthrough -D PRINTF_SUPPORT_LONG=1

$(LIB): $(OBJS)
	@mkdir -p $(MAKE_DIR)/libs
	@$(AR) cr $@ $^
	@echo "Archive $(notdir $@)"

%.o: %.c
	@$(CC) $(CFLAGS) -c $^ -o $@
	@echo "CC $@"

clean:
	@$(RM) -f $(LIB) $(OBJS)
	@$(RM) -f *.expand
	@echo "Remove objects: $(OBJS)"

.PHONY = clean
