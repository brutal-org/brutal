LIBS_BIN = $(BUILDDIR_CROSS)/libbrutal.a

LIBS_SRC = \
	$(wildcard sources/libs/syscalls/*.c) \
	$(wildcard sources/libs/ansi/*.c) \
	$(wildcard sources/libs/posix/*.c) \
	$(wildcard sources/libs/gnu/*.c) \
	$(wildcard sources/libs/bsd/*.c) \
	$(wildcard sources/libs/brutal/*.c) \
	$(wildcard sources/libs/brutal/*/*.c) \
	$(wildcard sources/libs/brutal/host/brutal/*.c)

LIBS_OBJ = $(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(LIBS_SRC))

DEPENDENCIES += $(LIBS_OBJ:.o=.d)

$(BUILDDIR_CROSS)/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_UCFLAGS)

$(LIBS_BIN): $(LIBS_OBJ)
	$(MKCWD)
	$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^
