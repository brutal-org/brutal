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
	$(patsubst sources/%, $(BINDIR_TARGET)/%.o, $(LIBS_SRC)) \
	$(patsubst $(GENDIR)/%, $(BINDIR_TARGET)/%.o, $(GENERATED_SRC))


LIBS_BIN=$(BINDIR_TARGET)/libbrutal.a

DEPENDENCIES += $(LIBS_OBJ:.o=.d)

$(BINDIR_TARGET)/%.c.o: $(GENDIR)/%.c | $(GENERATED_HDR) $(GENERATED_MOD)
	@$(MKCWD)
	$(USER_CC) -c -o $@ $< $(USER_UCFLAGS)

$(BINDIR_TARGET)/%.c.o: sources/%.c | $(GENERATED_HDR) $(GENERATED_MOD)
	@$(MKCWD)
	$(USER_CC) -c -o $@ $< $(USER_UCFLAGS)

$(BINDIR_TARGET)/%.s.o: sources/%.s
	@$(MKCWD)
	$(USER_AS) -o $@ $< $(USER_ASFLAGS)

$(LIBS_BIN): $(LIBS_OBJ)
	@$(MKCWD)
	$(USER_AR) $(USER_ARFLAGS) $@ $^

define BIN_TEMPLATE

$(1)_SRC = $$(wildcard $$($(1)_PATH)/*.c)
$(1)_OBJ = $$(patsubst sources/%,$(BINDIR_TARGET)/%.o, $$($(1)_SRC))
$(1)_BIN = $(BINDIR_TARGET)/$$($(1)_NAME)/$$($(1)_NAME)

$(1)_JSON = $(BINDIR_TARGET)/$$($(1)_NAME).json

ALL+=$$($(1)_BIN)

DEPENDENCIES += $$($(1)_OBJ:.o=.d)

$$($(1)_JSON): $$($(1)_PATH)/manifest.json $$(IDL_HOST_BIN) ./sources/build/scripts/manifest-builder.py
	@$$(MKCWD)
	./sources/build/scripts/manifest-builder.py $$($(1)_PATH)/manifest.json > $$@

$$($(1)_BIN): $$($(1)_OBJ) $(LIBS_BIN) $$($(1)_JSON)
	@$$(MKCWD)
	$(USER_LD) -o $$@ $$($(1)_OBJ) $(LIBS_BIN) $(USER_ULDFLAGS)
	$(USER_OBJCOPY) \
        --add-section .brutal.manifest=$$($(1)_JSON) \
        --set-section-flags .brutal.manifest=readonly,contents \
        $$@

endef

list-user:
	@echo $(TARGET_PKGS)

$(foreach bin, $(TARGET_PKGS), $(eval $(call BIN_TEMPLATE,$(bin))))
