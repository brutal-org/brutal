LIBS_HOST_SRC = \
	$(wildcard sources/embed/posix/*.c)    \
	$(wildcard sources/embed/posix/$(CONFIG_HOST_ARCH)/*.s) \
	$(wildcard sources/embed/posix/$(CONFIG_HOST_ARCH)/*.c) \
	$(wildcard sources/embed/sdl/*.c) \
	$(wildcard sources/embed/$(CONFIG_HOST_ARCH)/*.s) \
	$(wildcard sources/embed/$(CONFIG_HOST_ARCH)/*.c) \
	$(wildcard sources/libs/brutal/*.c)   \
	$(wildcard sources/libs/brutal/*/*.c) \
	$(wildcard sources/libs/brutal/*/*/*.c) \
	$(wildcard sources/libs/cc/*.c)       \
	$(wildcard sources/libs/cc/*/*.c)       \
	$(wildcard sources/libs/cc/*/*/*.c)       \
	$(wildcard sources/libs/ud/*.c)       \
	$(wildcard sources/libs/bvm/*.c)       \
	$(wildcard sources/libs/ud/*/*.c)       \
	$(wildcard sources/libs/json/*.c)     \
	$(wildcard sources/libs/elf/*.c)      \
	$(wildcard sources/libs/boot/abi/handover.c) \
	$(wildcard sources/libs/bid/*.c) \
	$(wildcard sources/libs/bid/*/*.c)

LIBS_HOST_OBJ = \
	$(patsubst sources/%, $(BINDIR_HOST)/%.o, $(LIBS_HOST_SRC))

LIBS_HOST_BIN=$(BINDIR_HOST)/libbrutal.a

$(BINDIR_HOST)/%.c.o: sources/%.c
	@$(MKCWD)
	$(HOST_CC) -c -o $@ $< $(HOST_CFLAGS)

$(BINDIR_HOST)/%.s.o: sources/%.s
	@$(MKCWD)
	$(USER_AS) -o $@ $< $(USER_ASFLAGS)

$(LIBS_HOST_BIN): $(LIBS_HOST_OBJ)
	@$(MKCWD)
	$(HOST_AR) $(HOST_ARFLAGS) $@ $^

DEPENDENCIES += $$(LIBS_HOST_OBJ:.o=.d)

define HOST_TEMPLATE

$(1)_HOST_SRC = \
	$$(wildcard sources/pkgs/$($(1)_NAME)/*.c) \
	$$(wildcard sources/pkgs/$($(1)_NAME)/*/*.c) \
	$$(wildcard sources/pkgs/$($(1)_NAME)/*/*/*.c)

$(1)_HOST_OBJ = $$(patsubst sources/%, $(BINDIR_HOST)/%.o, $$($(1)_HOST_SRC))

$(1)_HOST_BIN  = $(BINDIR_HOST)/$($(1)_NAME)
ALL+=$$($(1)_HOST_BIN)

DEPENDENCIES += $$($(1)_HOST_OBJ:.o=.d)

$$($(1)_HOST_BIN): $$($(1)_HOST_OBJ) $(LIBS_HOST_BIN)
	@$$(MKCWD)
	$(HOST_CC) -o $$@ $$^ $(HOST_LDFLAGS) $(HOST_CFLAGS)

endef

list-host:
	@echo $(HOST_PKGS)

$(foreach bin, $(HOST_PKGS), $(eval $(call HOST_TEMPLATE,$(bin))))
