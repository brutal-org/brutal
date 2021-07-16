BID_SRC= $(wildcard sources/bid/*.c) \
		 $(wildcard sources/bid/*/*.c) \
		 $(wildcard sources/bid/*/*/*.c) \
		 $(wildcard sources/libs/brutal/*.c) \
		 $(wildcard sources/libs/brutal/*/*.c) \
		 $(wildcard sources/libs/brutal/host/linux/*.c)

BID_OBJ=$(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(BID_SRC))
BID_BIN=$(BUILDDIR_HOST)/bid.elf

$(BUILDDIR_HOST)/%.c.o: sources/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) -fsanitize=address -fsanitize=undefined -c -o $@ $^

$(BID_BIN): $(BID_OBJ)
	$(MKCWD)
	$(HOST_CC) -rdynamic $(HOST_CFLAGS) $(HOST_LDFLAGS) -fsanitize=address -fsanitize=undefined $^ -o $@

run-bid: $(BID_BIN)
	$(BID_BIN) sources/bid/test.bid sources/bid/test.c

bid: $(BID_BIN)
