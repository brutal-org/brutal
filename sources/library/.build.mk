LIB_BIN = build/libbrutal.a

LIB_SRC = \
	$(wildcard sources/library/*.c) \
	$(wildcard sources/library/*/*.c)

LIB_OBJ = $(patsubst sources/%.c, build/%.c.o, $(LIB_SRC))

TARGETS += $(LIB_BIN)

build/library/%.c.o: sources/library/%.c
	$(MKCWD)
	$(CROSS_CC) $(CROSS_CFLAGS) -c -o $@ $^

$(LIB_BIN): $(LIB_OBJ)
	$(MKCWD)
	$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^
