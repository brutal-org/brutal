LIBS_SRC = \
	$(wildcard sources/embed/brutal/*.c)   \
	$(wildcard sources/embed/brutal/$(CONFIG_ARCH)/*.c) \
	$(wildcard sources/embed/brutal/$(CONFIG_ARCH)/*.s) \
	$(wildcard sources/embed/sysv/$(CONFIG_ARCH)/*.c)   \
	$(wildcard sources/embed/sysv/$(CONFIG_ARCH)/*.s)   \
	$(wildcard sources/embed/$(CONFIG_ARCH)/*.c) \
	$(wildcard sources/embed/$(CONFIG_ARCH)/*.s) \
	$(wildcard sources/libs/bal/*.c) \
	$(wildcard sources/libs/bal/*/*.c) \
	$(wildcard sources/libs/brutal/*.c)   \
	$(wildcard sources/libs/brutal/*/*.c) \
	$(wildcard sources/libs/cc/*.c)       \
	$(wildcard sources/libs/cc/*/*.c)       \
	$(wildcard sources/libs/elf/*.c)      \
	$(wildcard sources/libs/bal/abi/handover.c) \
	$(wildcard sources/libs/hw/acpi/*.c)     \
	$(wildcard sources/libs/hw/pci/*.c)      \
	$(wildcard sources/libs/json/*.c)     \
	$(wildcard sources/libs/stdc/*/*.c)     \
	$(wildcard sources/libs/ubsan/*.c)

LIBS_OBJ = \
	$(patsubst sources/%, $(BINDIR_CROSS)/%.o, $(LIBS_SRC))

LIBS_BIN=$(BINDIR_CROSS)/libbrutal.a

DEPENDENCIES += $(LIBS_OBJ:.o=.d)

$(BINDIR_CROSS)/%.c.o: sources/%.c
	$(ECHO) "brutal CC" $<
	@$(MKCWD)
	@$(CROSS_CC) -c -o $@ $< $(CROSS_UCFLAGS)

$(BINDIR_CROSS)/%.s.o: sources/%.s
	$(ECHO) "brutal AS" $<
	@$(MKCWD)
	@$(CROSS_AS) -o $@ $< $(CROSS_ASFLAGS)

$(LIBS_BIN): $(LIBS_OBJ)
	$(ECHO) "brutal AR" $@
	@$(MKCWD)
	@$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^

define BIN_TEMPLATE

$(1)_SRC = $$(wildcard sources/bins/$($(1)_NAME)/*.c)

$(1)_OBJ = $$(patsubst sources/%, $(BINDIR_CROSS)/%.o, $$($(1)_SRC))

$(1)_BIN  = $(BINDIR_CROSS)/$($(1)_NAME)

DEPENDENCIES += $$($(1)_OBJ:.o=.d)

SERVERS+=$$($(1)_BIN)
ALL+=$$($(1)_BIN)

$$($(1)_BIN): $$($(1)_OBJ) $(LIBS_BIN)
	$$(ECHO) "brutal LD" $$@
	@$$(MKCWD)
	@$(CROSS_LD) -o $$@ $$^ $(CROSS_ULDFLAGS)

endef

-include sources/bins/*/.build.mk

$(foreach bin, $(CROSS_BINS), $(eval $(call BIN_TEMPLATE,$(bin))))
