KERNEL_C_SRC+=$(wildcard sources/kernel/*.c) \
			  $(wildcard sources/kernel/$(CONFIG_ARCH)/*.c) \
			  $(wildcard sources/kernel/$(CONFIG_ARCH)/*/*.c)

KERNEL_S_SRC+=$(wildcard sources/kernel/$(CONFIG_ARCH)/*.s)

KERNEL_LIBS_SRC = \
	$(wildcard sources/libs/ansi/ctype.c)           \
	$(wildcard sources/libs/ansi/string.c)          \
	$(wildcard sources/libs/brutal/alloc/*.c)       \
	$(wildcard sources/libs/brutal/ds/*.c)          \
	$(wildcard sources/libs/brutal/hash/*.c)        \
	$(wildcard sources/libs/brutal/host/kernel/*.c) \
	$(wildcard sources/libs/brutal/io/buffer.c)     \
	$(wildcard sources/libs/brutal/io/fmt.c)        \
	$(wildcard sources/libs/brutal/io/print.c)      \
	$(wildcard sources/libs/brutal/io/scan.c)       \
	$(wildcard sources/libs/brutal/io/write.c)      \
	$(wildcard sources/libs/brutal/log/*.c)         \
	$(wildcard sources/libs/brutal/mem/*.c)         \
	$(wildcard sources/libs/brutal/sync/*.c)        \
	$(wildcard sources/libs/brutal/text/*.c)        \
	$(wildcard sources/libs/brutal/time/convert.c)  \
	$(wildcard sources/libs/elf/elf.c)              \
	$(wildcard sources/libs/handover/*.c)           \
	$(wildcard sources/libs/ubsan/*.c)

KERNEL_OBJ= \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst sources/%.s, $(BUILDDIR_CROSS)/%.s.o, $(KERNEL_S_SRC)) \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/kernel/%.c.o, $(KERNEL_LIBS_SRC)) \

KERNEL=$(BUILDDIR_CROSS)/kernel.elf

DEPENDENCIES += $(KERNEL_OBJ:.o=.d)

$(BUILDDIR_CROSS)/kernel/%.c.o: sources/kernel/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS) -fsanitize=undefined

$(BUILDDIR_CROSS)/kernel/%.s.o: sources/kernel/%.s
	$(MKCWD)
	$(CROSS_AS) -o $@ $< $(CROSS_ASFLAGS)

$(BUILDDIR_CROSS)/kernel/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS) -fsanitize=undefined

$(KERNEL): $(KERNEL_OBJ)
	$(MKCWD)
	$(CROSS_LD) -o $@ $^ $(CROSS_KLDFLAGS)
