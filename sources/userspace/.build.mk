define BIN_TEMPLATE
$(1)_BIN  = $(BUILDDIR_CROSS)/$($(1)_NAME)

$(1)_SRC = $$(wildcard sources/userspace/$($(1)_NAME)/*.c)

$(1)_OBJ = $$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $$($(1)_SRC))

DEPENDENCIES += $$($(1)_OBJ:.o=.d)

SERVERS+=$$($(1)_BIN)

$(BUILDDIR_CROSS)/userspace/$($(1)_NAME)/%.c.o: sources/userspace/$($(1)_NAME)/%.c
	$$(MKCWD)
	$(CROSS_CC) -c -o $$@ $$< $(CROSS_UCFLAGS)

$$($(1)_BIN): $$($(1)_OBJ) $(LIBS_BIN) $(CRTS_BIN)
	$$(MKCWD)
	$(CROSS_LD) -o $$@ $$< $(CRTS_BIN) $(LIBS_BIN) $(CROSS_ULDFLAGS)
endef

-include sources/userspace/*/.build.mk
$(foreach bin, $(BINS), $(eval $(call BIN_TEMPLATE,$(bin))))
