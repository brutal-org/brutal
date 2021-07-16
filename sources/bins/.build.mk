define BIN_TEMPLATE
$(1)_BIN  = $(BUILDDIR_CROSS)/$($(1)_NAME)

$(1)_SRC = $$(wildcard sources/bins/$($(1)_NAME)/*.c)

$(1)_OBJ = $$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $$($(1)_SRC))

DEPENDENCIES += $$($(1)_OBJ:.o=.d)

SERVERS+=$$($(1)_BIN)

$(BUILDDIR_CROSS)/bins/$($(1)_NAME)/%.c.o: sources/bins/$($(1)_NAME)/%.c
	$$(MKCWD)
	$(CROSS_CC) -c -o $$@ $$< $(CROSS_UCFLAGS)

$$($(1)_BIN): $$($(1)_OBJ)
	$$(MKCWD)
	$(CROSS_LD) -o $$@ $$< $(CROSS_ULDFLAGS)
endef

-include sources/bins/*/.build.mk
$(foreach bin, $(BINS), $(eval $(call BIN_TEMPLATE,$(bin))))
