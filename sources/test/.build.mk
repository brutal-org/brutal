TEST_SRC=$(wildcard sources/test/*.c) \
		 $(wildcard sources/test/*/*.c) \
		 $(wildcard sources/libc/brutal/*.c) \
		 $(wildcard sources/libc/brutal/*/*.c) \
		 $(wildcard sources/host/linux/*.c)

TEST_OBJ=$(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(TEST_SRC))
TEST_BIN=$(BUILDDIR_HOST)/test.elf

$(BUILDDIR_HOST)/%.c.o: sources/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) -c -o $@ $^

$(TEST_BIN): $(TEST_OBJ)
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) $(HOST_LDFLAGS) $^ -o $@

run-test: $(TEST_BIN)
	$(TEST_BIN)
