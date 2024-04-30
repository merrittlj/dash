LIB = $(MAKE_DIR)/libs/libembmath.a
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

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
