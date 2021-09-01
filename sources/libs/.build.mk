LIBS_HOSTED_BIN=$(BUILDDIR_HOST)/libbrutal.a

LIBS_HOST_SRC = \
	$(wildcard sources/libs/brutal/*.c)   \
	$(wildcard sources/libs/brutal/*/*.c) \
	$(wildcard sources/host/linux/*.c)    \
	$(wildcard sources/libs/cc/*.c)       \
	$(wildcard sources/libs/json/*.c)     \
	$(wildcard sources/libs/elf/*.c)      \
	$(wildcard sources/libs/handover/*.c) \
	$(wildcard sources/libs/bid/*.c)

LIBS_HOSTED_OBJ = $(patsubst sources/%.c, $(BUILDDIR_HOST)/%.c.o, $(LIBS_HOST_SRC))

DEPENDENCIES += $(LIBS_HOSTED_OBJ:.o=.d)

$(BUILDDIR_HOST)/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(HOST_CC) -c -o $@ $< $(HOST_CFLAGS)

$(LIBS_HOSTED_BIN): $(LIBS_HOSTED_OBJ)
	$(MKCWD)
	$(HOST_AR) $(HOST_ARFLAGS) $@ $^
