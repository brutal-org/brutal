BID=$(BUILDDIR_HOST)/utils/bid.elf

$(BUILDDIR_HOST)/utils/%.c.o: sources/utils/%.c
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) $(HOST_LDFLAGS) -c -o $@ $^

$(BUILDDIR_HOST)/utils/%.elf: $(BUILDDIR_HOST)/utils/%.c.o $(LIBS_HOSTED_BIN)
	$(MKCWD)
	$(HOST_CC) $(HOST_CFLAGS) $(HOST_LDFLAGS) -o $@ $^

run-bid: $(BID)
	$(BID) sources/libs/proto/exemple.bid sources/libs/proto/exemple.h
