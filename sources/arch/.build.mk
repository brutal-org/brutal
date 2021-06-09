KERNEL_C_SRC+=\
	$(wildcard sources/arch/x86_64/*.c) \
	$(wildcard sources/arch/x86_64/*/*.c)

KERNEL_S_SRC+=$(wildcard sources/arch/x86_64/*.s)

$(CROSS_BUILDDIR)/arch/%.c.o: sources/arch/%.c
	$(MKCWD)
	$(CROSS_CC) $(CROSS_KCFLAGS) -c -o $@ $<

$(CROSS_BUILDDIR)/arch/%.s.o: sources/arch/%.s
	$(MKCWD)
	$(CROSS_AS) $(CROSS_ASFLAGS) -o $@ $<