PROTOS:= \
	$(wildcard sources/protos/*.idl) \
	$(wildcard sources/protos/*/*.idl)

PROTOS_HDR:=$(patsubst sources/%.idl, $(GENDIR)/%.h, $(PROTOS))
PROTOS_SRC:=$(patsubst sources/%.idl, $(GENDIR)/%.c, $(PROTOS))

GENERATED_HDR += $(PROTOS_HDR)
GENERATED_SRC += $(PROTOS_SRC)

.SECONDARY: $(GENERATED_HDR)
$(GENDIR)/%.h: sources/%.idl $(IDL_HOST_BIN)
	@$(MKCWD)
	$(IDL_HOST_BIN) $< --header > $@

.SECONDARY: $(GENERATED_SRC)
$(GENDIR)/%.c: sources/%.idl $(IDL_HOST_BIN)
	@$(MKCWD)
	$(IDL_HOST_BIN) $< --source > $@


