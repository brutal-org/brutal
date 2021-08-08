BS_SRC= $(wildcard sources/udfore/*.c) \
		 $(wildcard sources/libs/brutal/*.c) \
		 $(wildcard sources/libs/brutal/*/*.c) \
		 $(wildcard sources/libs/udfore/*/*.c) \
		 $(wildcard sources/libs/udfore/*.c) \
		 $(wildcard sources/libs/brutal/host/linux/*.c)

BS_OBJ=$(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(BS_SRC))
BS_BIN=$(BUILDDIR_HOST)/bs.elf

$(BUILDDIR_HOST)/%.c.o: sources/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) -fsanitize=address -fsanitize=undefined -c -o $@ $^

$(BS_BIN): $(BS_OBJ)
	$(MKCWD)
	$(HOST_CC) -rdynamic $(HOST_CFLAGS) $(HOST_LDFLAGS) -fsanitize=address -fsanitize=undefined $^ -o $@

run-bs: $(BS_BIN)
	$(BS_BIN) sources/udfore/test.scm

bs: $(BS_BIN)
