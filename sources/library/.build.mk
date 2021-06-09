LIB_BIN = $(CROSS_BUILDDIR)/libbrutal.a

LIB_SRC = \
	$(wildcard sources/library/*.c) \
	$(wildcard sources/library/*/*.c)

LIB_OBJ = $(patsubst sources/%.c, $(CROSS_BUILDDIR)/%.c.o, $(LIB_SRC))

TARGETS += $(LIB_BIN)
DEPENDENCIES += $(LIB_OBJ:.o=.d)

$(CROSS_BUILDDIR)/library/%.c.o: sources/library/%.c
	$(MKCWD)
	$(CROSS_CC) $(CROSS_CFLAGS) -c -o $@ $<

$(LIB_BIN): $(LIB_OBJ)
	$(MKCWD)
	$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^
