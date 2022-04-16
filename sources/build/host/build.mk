LIBS_HOST_SRC = \
	$(wildcard sources/libs/embed/posix/*.c) \
	$(wildcard sources/libs/embed/sdl/*.c) \
	$(wildcard sources/libs/embed/$(HOST_ARCH)/*.s) \
	$(wildcard sources/libs/embed/$(HOST_ARCH)/*.c) \
	$(wildcard sources/libs/brutal-*/*.c) \
	$(wildcard sources/libs/codec-*/*.c) \
	$(wildcard sources/libs/cc/*.c) \
	$(wildcard sources/libs/cc/*/*.c) \
	$(wildcard sources/libs/cc/*/*/*.c) \
	$(wildcard sources/libs/ud/*.c) \
	$(wildcard sources/libs/ud/*/*.c) \
	$(wildcard sources/libs/bvm/*.c) \
	$(wildcard sources/libs/fs/*.c) \
	$(wildcard sources/libs/fs/*/*.c) \
	$(wildcard sources/libs/bvm/*/*.c) \
	$(wildcard sources/libs/json/*.c) \
	$(wildcard sources/libs/elf/*.c) \
	$(wildcard sources/libs/boot/abi/handover.c) \
	$(wildcard sources/libs/idl/*.c) \
	$(wildcard sources/libs/idl/*/*.c)

LIBS_HOST_OBJ = \
	$(patsubst sources/%, $(BINDIR_HOST)/%.o, $(LIBS_HOST_SRC))

LIBS_HOST_BIN=$(BINDIR_HOST)/libbrutal.a

$(BINDIR_HOST)/%.c.o: sources/%.c | $(GENERATED_MOD)
	@$(MKCWD)
	$(HOST_CC) -c -o $@ $< $(HOST_CFLAGS)

$(BINDIR_HOST)/%.s.o: sources/%.s
	@$(MKCWD)
	$(HOST_AS) -o $@ $< $(HOST_ASFLAGS)

$(LIBS_HOST_BIN): $(LIBS_HOST_OBJ)
	@$(MKCWD)
	$(HOST_AR) $(HOST_ARFLAGS) $@ $^

DEPENDENCIES += $(LIBS_HOST_OBJ:.o=.d)

define HOST_TEMPLATE

$(1)_NAME = $$(shell echo $(1) | tr A-Z a-z)

$(1)_HOST_SRC = \
	$$(wildcard sources/$$(PKG_$(1)_PATH)/*.c) \
	$$(wildcard sources/$$(PKG_$(1)_PATH)/*/*.c) \
	$$(wildcard sources/$$(PKG_$(1)_PATH)/*/*/*.c)

$(1)_HOST_OBJ = $$(patsubst sources/%,$(BINDIR_HOST)/%.o, $$($(1)_HOST_SRC))

$(1)_HOST_BIN  = $(BINDIR_HOST)/$$($(1)_NAME)
ALL+=$$($(1)_HOST_BIN)
HOST_NAMES+=$$($(1)_NAME)

DEPENDENCIES += $$($(1)_HOST_OBJ:.o=.d)

$$($(1)_HOST_BIN): $$($(1)_HOST_OBJ) $(LIBS_HOST_BIN)
	@$$(MKCWD)
	$(HOST_CC) -o $$@ $$^ $(HOST_LDFLAGS) $(HOST_CFLAGS)

host-$$($(1)_NAME)-dump:
	@echo "$$($(1)_HOST_BIN)"
	@echo "$$($(1)_HOST_OBJ)"
	@echo "$$($(1)_HOST_SRC)"

endef

HOST_PKGS=$(APPS) $(UTILS)

list-host:
	@echo $(HOST_NAMES)

$(foreach bin, $(HOST_PKGS), $(eval $(call HOST_TEMPLATE,$(bin))))
