define BIN_TEMPLATE

$(1)_SRC = $$(wildcard sources/bins/$($(1)_NAME)/*.c)

$(1)_BIN  = $(BUILDDIR_CROSS)/$($(1)_NAME)

$(1)_OBJ = $$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $$($(1)_SRC))

DEPENDENCIES += $$($(1)_OBJ:.o=.d)

SERVERS+=$$($(1)_BIN)

$(BUILDDIR_CROSS)/bins/$($(1)_NAME)/%.c.o: sources/bins/$($(1)_NAME)/%.c
	$$(MKCWD)
	$(CROSS_CC) -c -o $$@ $$< $(CROSS_UCFLAGS)

$$($(1)_BIN): $$($(1)_OBJ) $(LIBS_BIN) $(CRTS_BIN)
	$$(MKCWD)
	$(CROSS_LD) -o $$@ $$< $(CRTS_BIN) $(LIBS_BIN) $(CROSS_ULDFLAGS)

endef

-include sources/bins/*/.build.mk
$(foreach bin, $(BINS), $(eval $(call BIN_TEMPLATE,$(bin))))

# FIXME: Special cases for BID and CC

BID=$(BUILDDIR_HOST)/bins/bid.elf

UD=$(BUILDDIR_HOST)/bins/cc.elf

$(BUILDDIR_HOST)/bins/%.c.o: sources/bins/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) $(HOST_LDFLAGS) -c -o $@ $^

$(BUILDDIR_HOST)/bins/%.elf: $(BUILDDIR_HOST)/bins/%.c.o $(LIBS_HOSTED_BIN)
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) $(HOST_LDFLAGS) -o $@ $^

run-bid: $(BID)
	$(BID) sources/libs/proto/exemple.bid sources/libs/proto/exemple.h

run-cc: $(UD)
	$(UD) sources/bins/test.c
