LIB = $(MAKE_DIR)/libs/libhal.a
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

$(LIB): $(OBJS)
	@mkdir -p $(MAKE_DIR)/libs
	@$(AR) cr $@ $^
	@echo "Archive $(notdir $@)"

$(OBJS): $(SRCS)
	@$(CC) $(CFLAGS) -c $^
	@echo "CC $(OBJS)"

clean:
	@$(RM) -f $(LIB) $(OBJS)
	@$(RM) -f *.expand
	@echo "Remove objects: $(OBJS)"

.PHONY = clean
