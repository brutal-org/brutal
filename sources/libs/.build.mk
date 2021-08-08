LIBS_BIN = $(BUILDDIR_CROSS)/libbrutal.a

LIBS_SRC = \
	$(wildcard sources/libs/ansi/*.c)               \
	$(wildcard sources/libs/brutal/*.c)             \
	$(wildcard sources/libs/brutal/*/*.c)           \
	$(wildcard sources/libs/brutal/host/brutal/*.c) \
	$(wildcard sources/libs/udfore/*.c)                 \
	$(wildcard sources/libs/bsd/*.c)                \
	$(wildcard sources/libs/elf/*.c)                \
	$(wildcard sources/libs/gnu/*.c)                \
	$(wildcard sources/libs/handover/*.c)           \
	$(wildcard sources/libs/math/*.c)               \
	$(wildcard sources/libs/posix/*.c)              \
	$(wildcard sources/libs/syscalls/*.c)           \
	$(wildcard sources/libs/ubsan/*.c)

LIBS_OBJ = $(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(LIBS_SRC))

DEPENDENCIES += $(LIBS_OBJ:.o=.d)

$(BUILDDIR_CROSS)/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_UCFLAGS) -fsanitize=undefined

$(LIBS_BIN): $(LIBS_OBJ)
	$(MKCWD)
	$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^
