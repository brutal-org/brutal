BID_SRC= $(wildcard sources/bid/*.c) \
		 $(wildcard sources/libraries/brutal/*.c) \
		 $(wildcard sources/libraries/brutal/*/*.c) \
		 $(wildcard sources/libraries/bid/*/*.c) \
		 $(wildcard sources/libraries/bid/*.c) \
		 $(wildcard sources/libraries/brutal/host/linux/*.c)

BID_OBJ=$(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(BID_SRC))
BID_BIN=$(BUILDDIR_HOST)/bid.elf

$(BUILDDIR_HOST)/%.c.o: sources/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) -fsanitize=address -fsanitize=undefined -c -o $@ $^

$(BID_BIN): $(BID_OBJ)
	$(MKCWD)
	$(HOST_CC) -rdynamic $(HOST_CFLAGS) $(HOST_LDFLAGS) -fsanitize=address -fsanitize=undefined $^ -o $@

run-bid: $(BID_BIN)
	$(BID_BIN) sources/libraries/proto/exemple.bid sources/libraries/proto/exemple.h

bid: $(BID_BIN)
