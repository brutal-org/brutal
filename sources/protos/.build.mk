PROTOS:=\
	$(wildcard sources/protos/*.bid) \
	$(wildcard sources/protos/*/*.bid)

PROTOS_HEADERS:=$(patsubst %.bid, %.h, $(PROTOS))
PROTOS_SOURCES:=$(patsubst %.bid, %.c, $(PROTOS))

PROTOS_GENERATED:=$(PROTOS_HEADERS) $(PROTOS_SOURCES)

%.h: %.bid $(BID_HOST_BIN)
	@$(MKCWD)
	@$(ECHO) "brutal BID" $@
	@$(BID_HOST_BIN) $< --header > $@

%.c: %.bid $(BID_HOST_BIN)
	@$(MKCWD)
	@$(ECHO) "brutal BID" $@
	@$(BID_HOST_BIN) $< --source > $@

all-protos: $(PROTOS_GENERATED)
