BS_SRC= $(wildcard sources/bs/*.c) \
		 $(wildcard sources/libraries/brutal/*.c) \
		 $(wildcard sources/libraries/brutal/*/*.c) \
		 $(wildcard sources/libraries/bs/*/*.c) \
		 $(wildcard sources/libraries/bs/*.c) \
		 $(wildcard sources/libraries/brutal/host/linux/*.c)

BS_OBJ=$(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(BS_SRC))
BS_BIN=$(BUILDDIR_HOST)/bs.elf

$(BUILDDIR_HOST)/%.c.o: sources/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) -fsanitize=address -fsanitize=undefined -c -o $@ $^

$(BS_BIN): $(BS_OBJ)
	$(MKCWD)
	$(HOST_CC) -rdynamic $(HOST_CFLAGS) $(HOST_LDFLAGS) -fsanitize=address -fsanitize=undefined $^ -o $@

run-bs: $(BS_BIN)
	$(BS_BIN) sources/bs/test.scm

bs: $(BS_BIN)
