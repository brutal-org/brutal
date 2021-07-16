LIBS_BIN = $(BUILDDIR_CROSS)/libbrutal.a

LIBS_SRC = \
	$(wildcard sources/libs/brutal/*.c) \
	$(wildcard sources/libs/brutal/*/*.c)

LIBS_OBJ = $(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(LIBS_SRC))

DEPENDENCIES += $(LIBS_OBJ:.o=.d)

$(BUILDDIR_CROSS)/libs/brutal/%.c.o: sources/libs/brutal/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_CFLAGS)

$(LIBS_BIN): $(LIBS_OBJ)
	$(MKCWD)
	$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^
