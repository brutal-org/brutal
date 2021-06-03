LIB_BIN = build/libbrutal.a
LIB_SRC = $(wildcard sources/lib/*.c)
LIB_OBJ = $(patsubst sources/%.c, build/%.c.o, $(LIB_SRC))

TARGETS += $(LIB_BIN)

build/lib/%.c.o: sources/lib/%.c
	$(MKCWD)
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIB_BIN): $(LIB_OBJ)
	$(MKCWD)
	$(AR) $(ARFLAGS) $@ $^
