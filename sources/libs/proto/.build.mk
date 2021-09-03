

PROT_BID_SRC = 	$(wildcard sources/libs/proto/*.bid) \
	 		$(wildcard sources/libs/proto/*/*.bid)

PROT_BID_H = $(patsubst %.bid, %.h, $(PROT_BID_SRC))

%.h: %.bid $(BID_HOST_BIN)
	$(ECHO) "\e[37mhost   \e[92mBID\e[m" $@
	@$(MKCWD)
	@$(BID_HOST_BIN) $< $@

prot_headers: $(PROT_BID_H)
