KERNEL_C_SRC+=\
	$(wildcard sources/arch/$(CONFIG_ARCH)/*.c) \
	$(wildcard sources/arch/$(CONFIG_ARCH)/*/*.c)

KERNEL_S_SRC+=$(wildcard sources/arch/$(CONFIG_ARCH)/*.s)

$(BUILDDIR_CROSS)/arch/%.c.o: sources/arch/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/arch/%.s.o: sources/arch/%.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $< $(CROSS_ASFLAGS)
