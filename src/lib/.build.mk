LIB_BIN = build/libbrutal.a
LIB_SRC = $(wildcard src/lib/*.c)
LIB_OBJ = $(patsubst src/%.c, build/%.c.o, $(LIB_SRC))

TARGETS += $(LIB_BIN)

build/lib/%.c.o: src/lib/%.c
	$(MKCWD)
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIB_BIN): $(LIB_OBJ)
	$(MKCWD)
	$(AR) $(ARFLAGS) $@ $^
