GENDIR=bin/generated

PROTOS:= \
	$(wildcard sources/protos/*.bid) \
	$(wildcard sources/protos/*/*.bid)

PROTOS_HDR:=$(patsubst sources/%.bid, $(GENDIR)/%.h, $(PROTOS))
PROTOS_SRC:=$(patsubst sources/%.bid, $(GENDIR)/%.c, $(PROTOS))

PROTOS_GENERATED:=$(PROTOS_HDR) $(PROTOS_SRC)

$(GENDIR)/%.h: sources/%.bid $(BID_HOST_BIN)
	@$(MKCWD)
	@$(ECHO) "brutal BID" $@
	@$(BID_HOST_BIN) $< --header > $@

$(GENDIR)/%.c: sources/%.bid $(BID_HOST_BIN)
	@$(MKCWD)
	@$(ECHO) "brutal BID" $@
	@$(BID_HOST_BIN) $< --source > $@

all-protos: $(PROTOS_GENERATED)
