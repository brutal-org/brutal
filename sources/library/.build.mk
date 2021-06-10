LIB_BIN = $(BUILDDIR_CROSS)/libbrutal.a

LIB_SRC = \
	$(wildcard sources/library/*.c) \
	$(wildcard sources/library/*/*.c)

LIB_OBJ = $(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(LIB_SRC))

TARGETS += $(LIB_BIN)
DEPENDENCIES += $(LIB_OBJ:.o=.d)

$(BUILDDIR_CROSS)/library/%.c.o: sources/library/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_CFLAGS)

$(LIB_BIN): $(LIB_OBJ)
	$(MKCWD)
	$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^
