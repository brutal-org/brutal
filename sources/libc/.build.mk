LIBC_BIN = $(BUILDDIR_CROSS)/libbrutal.a

LIBC_SRC = \
	$(wildcard sources/libc/brutal/*.c) \
	$(wildcard sources/libc/brutal/*/*.c)

LIBC_OBJ = $(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(LIBC_SRC))

TARGETS += $(LIBC_BIN)
DEPENDENCIES += $(LIBC_OBJ:.o=.d)

$(BUILDDIR_CROSS)/libc/brutal/%.c.o: sources/libc/brutal/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_CFLAGS)

$(LIBC_BIN): $(LIBC_OBJ)
	$(MKCWD)
	$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^
