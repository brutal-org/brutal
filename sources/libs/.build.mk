LIBS_BIN = $(BUILDDIR_CROSS)/libbrutal.a

LIBS_SRC = \
	$(wildcard sources/libs/ansi/*.c)               \
	$(wildcard sources/libs/brutal/*.c)             \
	$(wildcard sources/libs/brutal/*/*.c)           \
	$(wildcard sources/libs/brutal/host/brutal/*.c) \
	$(wildcard sources/libs/udfore/*.c)             \
	$(wildcard sources/libs/bsd/*.c)                \
	$(wildcard sources/libs/elf/*.c)                \
	$(wildcard sources/libs/gnu/*.c)                \
	$(wildcard sources/libs/handover/*.c)           \
	$(wildcard sources/libs/math/*.c)               \
	$(wildcard sources/libs/json/*.c)               \
	$(wildcard sources/libs/posix/*.c)              \
	$(wildcard sources/libs/syscalls/*.c)           \
	$(wildcard sources/libs/ubsan/*.c)

LIBS_OBJ = $(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(LIBS_SRC))

DEPENDENCIES += $(LIBS_OBJ:.o=.d)

$(BUILDDIR_CROSS)/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_UCFLAGS)

$(LIBS_BIN): $(LIBS_OBJ)
	$(MKCWD)
	$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^

LIBS_HOSTED_BIN=$(BUILDDIR_HOST)/libbrutal.a

LIBS_HOST_SRC = \
	$(wildcard sources/libs/brutal/*.c)             \
	$(wildcard sources/libs/brutal/*/*.c)           \
	$(wildcard sources/libs/brutal/host/linux/*.c)  \
	$(wildcard sources/libs/udfore/*.c)             \
	$(wildcard sources/libs/json/*.c)             \
	$(wildcard sources/libs/elf/*.c)                \
	$(wildcard sources/libs/handover/*.c)           \
	$(wildcard sources/libs/bid/*.c)

LIBS_HOSTED_OBJ = $(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(LIBS_HOST_SRC))

DEPENDENCIES += $(LIBS_HOSTED_OBJ:.o=.d)

$(BUILDDIR_HOST)/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(HOST_CC) -c -o $@ $< $(HOST_CFLAGS)

$(LIBS_HOSTED_BIN): $(LIBS_HOSTED_OBJ)
	$(MKCWD)
	$(HOST_AR) $(HOST_ARFLAGS) $@ $^
