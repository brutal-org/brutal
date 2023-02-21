LIBS_SRC = \
	$(wildcard sources/libs/embed/brutal/*.c) \
	$(wildcard sources/libs/embed/brutal/$(ARCH)/*.c) \
	$(wildcard sources/libs/embed/brutal/$(ARCH)/*.s) \
	$(wildcard sources/libs/embed/$(ARCH)/*.c) \
	$(wildcard sources/libs/embed/$(ARCH)/*.s) \
	$(wildcard sources/libs/ipc/*.c) \
	$(wildcard sources/libs/ipc/*/*.c) \
	$(wildcard sources/libs/bal/*.c) \
	$(wildcard sources/libs/bal/*/*.c) \
	$(wildcard sources/libs/brutal-*/*.c) \
	$(wildcard sources/libs/codec-*/*.c) \
	$(wildcard sources/libs/cc/*.c) \
	$(wildcard sources/libs/cc/*/*.c) \
	$(wildcard sources/libs/cc/*/*/*.c) \
	$(wildcard sources/libs/elf/*.c) \
	$(wildcard sources/libs/hw/acpi/*.c) \
	$(wildcard sources/libs/hw/fdt/*.c) \
	$(wildcard sources/libs/hw/pci/*.c) \
	$(wildcard sources/libs/hw/ahci/*.c) \
	$(wildcard sources/libs/hw/ps2/*.c) \
	$(wildcard sources/libs/json/*.c) \
	$(wildcard sources/libs/stdc-*/*.c) \
	$(wildcard sources/libs/absan/*.c) \
	$(wildcard sources/libs/ubsan/*.c)

LIBS_OBJ = \
	$(patsubst sources/%, $(BINDIR_USER)/%.o, $(LIBS_SRC)) \
	$(patsubst $(GENDIR)/%, $(BINDIR_USER)/%.o, $(GENERATED_SRC))


LIBS_BIN=$(BINDIR_USER)/libbrutal.a

DEPENDENCIES += $(LIBS_OBJ:.o=.d)

$(BINDIR_USER)/%.c.o: $(GENDIR)/%.c | $(GENERATED_HDR) $(GENERATED_MOD)
	@$(MKCWD)
	$(USER_CC) -c -o $@ $< $(USER_UCFLAGS)

$(BINDIR_USER)/%.c.o: sources/%.c | $(GENERATED_HDR) $(GENERATED_MOD)
	@$(MKCWD)
	$(USER_CC) -c -o $@ $< $(USER_UCFLAGS)

$(BINDIR_USER)/%.s.o: sources/%.s
	@$(MKCWD)
	$(USER_AS) -o $@ $< $(USER_ASFLAGS)

$(LIBS_BIN): $(LIBS_OBJ)
	@$(MKCWD)
	$(USER_AR) $(USER_ARFLAGS) $@ $^

define BIN_TEMPLATE

$(1)_NAME = $$(shell echo $(1) | tr A-Z a-z)

$(1)_PKG = sources/$$(PKG_$(1)_PATH)

$(1)_SRC = $$(wildcard $$($(1)_PKG)/*.c)

$(1)_OBJ = $$(patsubst sources/%,$(BINDIR_USER)/%.o, $$($(1)_SRC))

$(1)_BIN = $(BINDIR_USER)/$$($(1)_NAME)

$(1)_JSON = $(BINDIR_USER)/$$($(1)_NAME).json

DEPENDENCIES += $$($(1)_OBJ:.o=.d)

PKGS+=$$($(1)_BIN)
ALL+=$$($(1)_BIN)

$$($(1)_JSON): $$($(1)_PKG)/manifest.json $$(IDL_HOST_BIN) ./sources/build/scripts/manifest-builder.py
	@$$(MKCWD)
	./sources/build/scripts/manifest-builder.py $$($(1)_PKG)/manifest.json > $$@

$$($(1)_BIN): $$($(1)_OBJ) $(LIBS_BIN) $$($(1)_JSON)
	@$$(MKCWD)
	$(USER_LD) -o $$@ $$($(1)_OBJ) $(LIBS_BIN) $(USER_ULDFLAGS)
	$(USER_OBJCOPY) \
        --add-section .brutal.manifest=$$($(1)_JSON) \
        --set-section-flags .brutal.manifest=readonly,contents \
        $$@


target-$$($(1)_NAME)-dump:
	@echo "$$($(1)_BIN)"
	@echo "$$($(1)_SRC)"
	@echo "$$($(1)_OBJ)"

endef

USER_PKGS:=$(APPS) $(SRVS)

list-user:
	@echo $(USER_PKGS)

$(foreach bin, $(USER_PKGS), $(eval $(call BIN_TEMPLATE,$(bin))))
