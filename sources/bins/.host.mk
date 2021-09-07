BUILDDIR_HOST=$(BUILDDIR)/host

LIBS_HOST_SRC = \
	$(wildcard sources/host/linux/*.c)    \
	$(wildcard sources/host/linux/$(CONFIG_ARCH)/*.s) \
	$(wildcard sources/host/linux/$(CONFIG_ARCH)/*.c) \
	$(wildcard sources/host/sysv/$(CONFIG_ARCH)/*.s) \
	$(wildcard sources/host/sysv/$(CONFIG_ARCH)/*.c) \
	$(wildcard sources/libs/brutal/*.c)   \
	$(wildcard sources/libs/brutal/*/*.c) \
	$(wildcard sources/libs/cc/*.c)       \
	$(wildcard sources/libs/json/*.c)     \
	$(wildcard sources/libs/elf/*.c)      \
	$(wildcard sources/libs/handover/*.c) \
	$(wildcard sources/libs/bid/*.c)

LIBS_HOST_OBJ = \
	$(patsubst sources/%, $(BUILDDIR_HOST)/%.o, $(LIBS_HOST_SRC))

LIBS_HOST_BIN=$(BUILDDIR_HOST)/libbrutal.a

$(BUILDDIR_HOST)/%.c.o: sources/%.c
	$(ECHO) "\033[37mhost   \033[92mCC\033[m" $<
	@$(MKCWD)
	@$(HOST_CC) -c -o $@ $^ $(HOST_CFLAGS)

$(BUILDDIR_HOST)/%.s.o: sources/%.s
	$(ECHO) "\033[37mhost   \033[92mAS\033[m" $<
	@$(MKCWD)
	@$(CROSS_AS) -o $@ $^ $(CROSS_ASFLAGS)

$(LIBS_HOST_BIN): $(LIBS_HOST_OBJ)
	$(ECHO) "\033[37mhost   \033[92mAR\033[m" $@
	@$(MKCWD)
	@$(HOST_AR) $(HOST_ARFLAGS) $@ $^

DEPENDENCIES += $$(LIBS_HOST_OBJ:.o=.d)

define HOST_TEMPLATE

$(1)_HOST_SRC = \
	$$(wildcard sources/bins/$($(1)_NAME)/*.c) \
	$$(wildcard sources/bins/$($(1)_NAME)/*/*.c) \
	$$(wildcard sources/bins/$($(1)_NAME)/*/*/*.c)

$(1)_HOST_OBJ = $$(patsubst sources/%, $(BUILDDIR_HOST)/%.o, $$($(1)_HOST_SRC))

$(1)_HOST_BIN  = $(BUILDDIR_HOST)/$($(1)_NAME)

DEPENDENCIES += $$($(1)_OBJ:.o=.d)

$$($(1)_HOST_BIN): $$($(1)_HOST_OBJ) $(LIBS_HOST_BIN)
	$$(ECHO) "\033[37mhost   \033[92mLD\033[m" $$@
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
	cd $(BUILDDIR_HOST); ./test

.PHONY: run-demo
run-demo: $(DEMO_HOST_BIN)
	$(DEMO_HOST_BIN)
