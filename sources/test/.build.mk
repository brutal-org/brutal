TEST_SRC=$(wildcard sources/test/*.c) \
		 $(wildcard sources/test/*/*.c) \
		 $(wildcard sources/library/*.c) \
		 $(wildcard sources/library/*/*.c) \
		 $(wildcard sources/host/linux/*.c)

TEST_OBJ=$(patsubst sources/%.c, $(HOST_BUILDDIR)/%.c.o, $(TEST_SRC))
TEST_BIN=$(HOST_BUILDDIR)/test.elf

$(HOST_BUILDDIR)/%.c.o: sources/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) -c -o $@ $^

$(TEST_BIN): $(TEST_OBJ)
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) $(HOST_LDFLAGS) $^ -o $@

run-test: $(TEST_BIN)
	$(TEST_BIN)