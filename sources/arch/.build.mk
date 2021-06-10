KERNEL_C_SRC+=\
	$(wildcard sources/arch/x86_64/*.c) \
	$(wildcard sources/arch/x86_64/*/*.c)

KERNEL_S_SRC+=$(wildcard sources/arch/x86_64/*.s)

$(BUILDDIR_CROSS)/arch/%.c.o: sources/arch/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/arch/%.s.o: sources/arch/%.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $< $(CROSS_ASFLAGS)
