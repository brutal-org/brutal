BINDIR_HOST=$(BINDIR)/host

LIBS_HOST_SRC = \
	$(wildcard sources/host/linux/*.c)    \
	$(wildcard sources/host/linux/$(CONFIG_ARCH)/*.s) \
	$(wildcard sources/host/linux/$(CONFIG_ARCH)/*.c) \
	$(wildcard sources/host/sysv/$(CONFIG_ARCH)/*.s) \
	$(wildcard sources/host/sysv/$(CONFIG_ARCH)/*.c) \
	$(wildcard sources/libs/brutal/*.c)   \
	$(wildcard sources/libs/brutal/*/*.c) \
	$(wildcard sources/libs/cc/*.c)       \
	$(wildcard sources/libs/cc/*/*.c)       \
	$(wildcard sources/libs/udfore/*.c)       \
	$(wildcard sources/libs/udfore/*/*.c)       \
	$(wildcard sources/libs/json/*.c)     \
	$(wildcard sources/libs/elf/*.c)      \
	$(wildcard sources/libs/handover/*.c) \
	$(wildcard sources/libs/bid/*.c)

LIBS_HOST_OBJ = \
	$(patsubst sources/%, $(BINDIR_HOST)/%.o, $(LIBS_HOST_SRC))

LIBS_HOST_BIN=$(BINDIR_HOST)/libbrutal.a

$(BINDIR_HOST)/%.c.o: sources/%.c
	$(ECHO) "host   CC" $<
	@$(MKCWD)
	@$(HOST_CC) -c -o $@ $< $(HOST_CFLAGS)

$(BINDIR_HOST)/%.s.o: sources/%.s
	$(ECHO) "host   AS" $<
	@$(MKCWD)
	@$(CROSS_AS) -o $@ $< $(CROSS_ASFLAGS)

$(LIBS_HOST_BIN): $(LIBS_HOST_OBJ)
	$(ECHO) "host   AR" $@
	@$(MKCWD)
	@$(HOST_AR) $(HOST_ARFLAGS) $@ $^

DEPENDENCIES += $$(LIBS_HOST_OBJ:.o=.d)

define HOST_TEMPLATE

$(1)_HOST_SRC = \
	$$(wildcard sources/bins/$($(1)_NAME)/*.c) \
	$$(wildcard sources/bins/$($(1)_NAME)/*/*.c) \
	$$(wildcard sources/bins/$($(1)_NAME)/*/*/*.c)

$(1)_HOST_OBJ = $$(patsubst sources/%, $(BINDIR_HOST)/%.o, $$($(1)_HOST_SRC))

$(1)_HOST_BIN  = $(BINDIR_HOST)/$($(1)_NAME)

DEPENDENCIES += $$($(1)_HOST_OBJ:.o=.d)

$$($(1)_HOST_BIN): $$($(1)_HOST_OBJ) $(LIBS_HOST_BIN)
	$$(ECHO) "host   LD" $$@
	@$$(MKCWD)
	$(HOST_CC) -o $$@ $$^ $(HOST_LDFLAGS) $(HOST_CFLAGS)

endef

$(foreach bin, $(HOST_BINS), $(eval $(call HOST_TEMPLATE,$(bin))))

.PHONY: run-bid
run-bid: $(BID_HOST_BIN)
	$(BID_HOST_BIN) sources/libs/proto/exemple.bid sources/libs/proto/exemple.h

.PHONY: run-cc
run-cc: $(CC_HOST_BIN)
	$(CC_HOST_BIN) sources/bins/test.c

.PHONY: run-test
run-test: $(TEST_HOST_BIN)
	cd $(BINDIR_HOST); ./test

.PHONY: run-demo
run-demo: $(DEMO_HOST_BIN)
	$(DEMO_HOST_BIN)

.PHONY: run-ud
run-ud: $(UD_HOST_BIN)
	$(UD_HOST_BIN) sources/bins/ud/test.ud.exemple
