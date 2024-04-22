LIB = $(MAKE_DIR)/libs/libhal.a
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

$(LIB): $(OBJS)
	@mkdir -p $(MAKE_DIR)/libs
	@$(AR) cr $@ $^
	@echo "Archive $(notdir $@)"

%.o: %.c
	@echo "make: $(MAKE_DIR)"
	@$(CC) $(CFLAGS) -c $^ -o $@
	@echo "CC $@"

clean:
	@$(RM) -f $(LIB) $(OBJS)
	@$(RM) -f *.expand
	@echo "Remove objects: $(OBJS)"

.PHONY = clean
