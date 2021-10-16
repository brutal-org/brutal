BUILDDIR_CROSS = $(BUILDDIR)/cross

LIBS_SRC = \
	$(wildcard sources/host/brutal/*.c)   \
	$(wildcard sources/host/brutal/$(CONFIG_ARCH)/*.s) \
	$(wildcard sources/host/brutal/$(CONFIG_ARCH)/*.c) \
	$(wildcard sources/host/sysv/$(CONFIG_ARCH)/*.s)   \
	$(wildcard sources/host/sysv/$(CONFIG_ARCH)/*.c)   \
	$(wildcard sources/libs/acpi/*.c)     \
	$(wildcard sources/libs/brutal/*.c)   \
	$(wildcard sources/libs/brutal/*/*.c) \
	$(wildcard sources/libs/cc/*.c)       \
	$(wildcard sources/libs/cc/*/*.c)       \
	$(wildcard sources/libs/elf/*.c)      \
	$(wildcard sources/libs/pci/*.c)      \
	$(wildcard sources/libs/handover/*.c) \
	$(wildcard sources/libs/json/*.c)     \
	$(wildcard sources/libs/bal/*.c) \
	$(wildcard sources/libs/stdc/*/*.c)     \
	$(wildcard sources/libs/ubsan/*.c)

LIBS_OBJ = \
	$(patsubst sources/%, $(BUILDDIR_CROSS)/%.o, $(LIBS_SRC))

LIBS_BIN=$(BUILDDIR_CROSS)/libbrutal.a

DEPENDENCIES += $(LIBS_OBJ:.o=.d)

$(BUILDDIR_CROSS)/%.c.o: sources/%.c
	$(ECHO) "\e[37mbrutal \e[92mCC\e[m" $<
	@$(MKCWD)
	@$(CROSS_CC) -c -o $@ $< $(CROSS_UCFLAGS)

$(BUILDDIR_CROSS)/%.s.o: sources/%.s
	$(ECHO) "\e[37mbrutal \e[92mAS\e[m" $<
	@$(MKCWD)
	@$(CROSS_AS) -o $@ $< $(CROSS_ASFLAGS)

$(LIBS_BIN): $(LIBS_OBJ)
	$(ECHO) "\e[37mbrutal \e[92mAR\e[m" $@
	@$(MKCWD)
	@$(CROSS_AR) $(CROSS_ARFLAGS) $@ $^

define BIN_TEMPLATE

$(1)_SRC = $$(wildcard sources/bins/$($(1)_NAME)/*.c)

$(1)_OBJ = $$(patsubst sources/%, $(BUILDDIR_CROSS)/%.o, $$($(1)_SRC))

$(1)_BIN  = $(BUILDDIR_CROSS)/$($(1)_NAME)

DEPENDENCIES += $$($(1)_OBJ:.o=.d)

SERVERS+=$$($(1)_BIN)

$$($(1)_BIN): $$($(1)_OBJ) $(LIBS_BIN)
	$$(ECHO) "\e[37mbrutal \e[92mLD\e[m" $$@
	@$$(MKCWD)
	@$(CROSS_LD) -o $$@ $$^ $(CROSS_ULDFLAGS)

endef

-include sources/bins/*/.build.mk

$(foreach bin, $(CROSS_BINS), $(eval $(call BIN_TEMPLATE,$(bin))))
