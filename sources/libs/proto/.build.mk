

PROT_BID_SRC = 	$(wildcard sources/libs/proto/*.bid) \
	 		$(wildcard sources/libs/proto/*/*.bid)

PROT_BID_H = $(patsubst %.bid, %.h, $(PROT_BID_SRC))

%.h: %.bid $(BID_HOST_BIN)
	$(ECHO) "host   BID" $@
	@$(MKCWD)
	$(V)$(BID_HOST_BIN) $< $@

prot_headers: $(PROT_BID_H)
