CRTS_BIN= \
	$(BUILDDIR_CROSS)/crt0.o \
	$(BUILDDIR_CROSS)/crti.o \
	$(BUILDDIR_CROSS)/crtn.o \
	$(BUILDDIR_CROSS)/fibers.o \
	$(BUILDDIR_CROSS)/entry.o

$(BUILDDIR_CROSS)/crt0.o: sources/archs/$(CONFIG_ARCH)/crt0.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $^ $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/crti.o: sources/archs/$(CONFIG_ARCH)/crti.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $^ $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/crtn.o: sources/archs/$(CONFIG_ARCH)/crtn.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $^ $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/fibers.o: sources/archs/$(CONFIG_ARCH)/fibers.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $^ $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/entry.o: sources/archs/entry.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $^ $(CROSS_UCFLAGS)
