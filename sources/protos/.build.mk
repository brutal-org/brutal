PROTOS:=\
	$(wildcard sources/protos/*.bid) \
	$(wildcard sources/protos/*/*.bid)

PROTOS_HEADERS:=$(patsubst sources/%.bid, $(BINDIR_CROSS)/%.h, $(PROTOS))
PROTOS_SOURCES:=$(patsubst sources/%.bid, $(BINDIR_CROSS)/%.c, $(PROTOS))

PROTOS_GENERATED:=$(PROTOS_HEADERS) $(PROTOS_SOURCES)

$(BINDIR_CROSS)/%.h: sources/%.bid | $(BID_BIN)
	$(BID_BIN) $< --header > $@

$(BINDIR_CROSS)/%.c: sources/%.bid | $(BID_BIN)
	$(BID_BIN) $< --source > $@

all-protos: $(PROTOS_GENERATED)
