CRTS_BIN= \
	$(BUILDDIR_CROSS)/crt0.o \
	$(BUILDDIR_CROSS)/crti.o \
	$(BUILDDIR_CROSS)/crtn.o \
	$(BUILDDIR_CROSS)/entry.o

$(BUILDDIR_CROSS)/crt0.o: sources/crts/$(CONFIG_ARCH)/crt0.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $^ $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/crti.o: sources/crts/$(CONFIG_ARCH)/crti.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $^ $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/crtn.o: sources/crts/$(CONFIG_ARCH)/crtn.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $^ $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/entry.o: sources/crts/entry.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $^ $(CROSS_UCFLAGS)
