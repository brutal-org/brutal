GENDIR=bin/generated

PROTOS:= \
	$(wildcard sources/protos/*.bid) \
	$(wildcard sources/protos/*/*.bid)

PROTOS_HDR:=$(patsubst sources/%.bid, $(GENDIR)/%.h, $(PROTOS))
PROTOS_SRC:=$(patsubst sources/%.bid, $(GENDIR)/%.c, $(PROTOS))

GENERATED_HDR += $(PROTOS_HDR)
GENERATED_SRC += $(PROTOS_SRC)

$(GENDIR)/%.h: sources/%.bid $(BID_HOST_BIN)
	@$(MKCWD)
	$(BID_HOST_BIN) $< --header > $@

$(GENDIR)/%.c: sources/%.bid $(BID_HOST_BIN)
	@$(MKCWD)
	$(BID_HOST_BIN) $< --source > $@
