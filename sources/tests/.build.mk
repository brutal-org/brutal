TEST_SRC= \
	$(wildcard sources/tests/*.c)                       \
	$(wildcard sources/tests/*/*.c)                     \
	$(wildcard sources/tests/*/*/*.c)                   \
	$(wildcard sources/libraries/brutal/*.c)            \
	$(wildcard sources/libraries/brutal/*/*.c)          \
	$(wildcard sources/libraries/bid/*/*.c)             \
	$(wildcard sources/libraries/bid/*.c)               \
	$(wildcard sources/libraries/brutal/host/linux/*.c)

TEST_OBJ=$(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(TEST_SRC))
TEST_BIN=$(BUILDDIR_HOST)/test.elf

$(BUILDDIR_HOST)/%.c.o: sources/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) -fsanitize=address -fsanitize=undefined -c -o $@ $^

$(TEST_BIN): $(TEST_OBJ)
	$(MKCWD)
	$(HOST_CC) -rdynamic $(HOST_CFLAGS) $(HOST_LDFLAGS) -fsanitize=address -fsanitize=undefined $^ -o $@

run-test: $(TEST_BIN)
	$(TEST_BIN)
