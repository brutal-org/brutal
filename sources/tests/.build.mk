TEST_SRC= \
	$(wildcard sources/tests/*.c) \
	$(wildcard sources/tests/*/*.c) \
	$(wildcard sources/tests/*/*/*.c)

TEST_OBJ=$(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(TEST_SRC))
TEST_BIN=$(BUILDDIR_HOST)/test.elf

$(BUILDDIR_HOST)/%.c.o: sources/%.c
	$(MKCWD)
	$(HOST_CC) -c -o $@ $^ $(HOST_CFLAGS)

$(TEST_BIN): $(TEST_OBJ) $(LIBS_HOSTED_BIN)
	$(MKCWD)
	$(HOST_CC) -o $@ $^ $(HOST_CFLAGS) $(HOST_LDFLAGS)

run-test: $(TEST_BIN)
	cd $(BUILDDIR_HOST); ./test.elf
