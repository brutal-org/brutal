KERNEL_C_SRC+=$(wildcard sources/kernel/*.c) \
			  $(wildcard sources/kernel/*/*.c)

KERNEL_LIBS_SRC = \
			  $(wildcard sources/libs/ansi/*.c) \
			  $(wildcard sources/libs/ubsan/*.c) \
			  $(wildcard sources/libs/brutal/io/fmt.c) \
			  $(wildcard sources/libs/brutal/io/print.c) \
			  $(wildcard sources/libs/brutal/io/scan.c) \
			  $(wildcard sources/libs/brutal/io/write.c) \
			  $(wildcard sources/libs/brutal/mem/*.c) \
			  $(wildcard sources/libs/brutal/text/*.c) \
			  $(wildcard sources/libs/brutal/sync/*.c) \
			  $(wildcard sources/libs/brutal/ds/*.c) \
			  $(wildcard sources/libs/brutal/log/*.c) \
			  $(wildcard sources/libs/elf/*.c) \
			  $(wildcard sources/libs/handover/*.c) \
			  $(wildcard sources/libs/ansi/string.c) \
			  $(wildcard sources/libs/brutal/host/kernel/*.c) \
			  $(wildcard sources/libs/brutal/alloc/*.c)

KERNEL_OBJ= \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/kernel/%.c.o, $(KERNEL_LIBS_SRC)) \
	$(patsubst sources/%.s, $(BUILDDIR_CROSS)/%.s.o, $(KERNEL_S_SRC))

KERNEL=$(BUILDDIR_CROSS)/kernel.elf

DEPENDENCIES += $(KERNEL_OBJ:.o=.d)

$(BUILDDIR_CROSS)/kernel/%.c.o: sources/kernel/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/kernel/libs/brutal/%.c.o: sources/libs/brutal/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/kernel/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(KERNEL): $(KERNEL_OBJ)
	$(MKCWD)
	$(CROSS_LD) -o $@ $^ $(CROSS_KLDFLAGS)
