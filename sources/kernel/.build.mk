KERNEL_C_SRC+=$(wildcard sources/kernel/*.c) \
			  $(wildcard sources/kernel/$(CONFIG_ARCH)/*.c) \
			  $(wildcard sources/kernel/$(CONFIG_ARCH)/*/*.c)

KERNEL_S_SRC+=$(wildcard sources/kernel/$(CONFIG_ARCH)/*.s)

KERNEL_LIBS_SRC = \
			  $(wildcard sources/libraries/ansi/ctype.c) \
			  $(wildcard sources/libraries/ubsan/*.c) \
			  $(wildcard sources/libraries/brutal/io/fmt.c) \
			  $(wildcard sources/libraries/brutal/io/print.c) \
			  $(wildcard sources/libraries/brutal/io/scan.c) \
			  $(wildcard sources/libraries/brutal/io/write.c) \
			  $(wildcard sources/libraries/brutal/io/buffer.c) \
			  $(wildcard sources/libraries/brutal/mem/*.c) \
			  $(wildcard sources/libraries/brutal/text/*.c) \
			  $(wildcard sources/libraries/brutal/sync/*.c) \
			  $(wildcard sources/libraries/brutal/ds/*.c) \
			  $(wildcard sources/libraries/brutal/log/*.c) \
			  $(wildcard sources/libraries/elf/*.c) \
			  $(wildcard sources/libraries/handover/*.c) \
			  $(wildcard sources/libraries/ansi/string.c) \
			  $(wildcard sources/libraries/brutal/host/kernel/*.c) \
			  $(wildcard sources/libraries/brutal/alloc/*.c)

KERNEL_OBJ= \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst sources/%.s, $(BUILDDIR_CROSS)/%.s.o, $(KERNEL_S_SRC)) \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/kernel/%.c.o, $(KERNEL_LIBS_SRC)) \

KERNEL=$(BUILDDIR_CROSS)/kernel.elf

DEPENDENCIES += $(KERNEL_OBJ:.o=.d)

$(BUILDDIR_CROSS)/kernel/%.c.o: sources/kernel/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/kernel/%.s.o: sources/kernel/%.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $< $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/kernel/libraries/brutal/%.c.o: sources/libraries/brutal/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/kernel/libraries/%.c.o: sources/libraries/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(KERNEL): $(KERNEL_OBJ)
	$(MKCWD)
	$(CROSS_LD) -o $@ $^ $(CROSS_KLDFLAGS)
