KERNEL_C_SRC+=$(wildcard sources/kernel/*.c) \
			  $(wildcard sources/kernel/*/*.c)

KERNEL_LIB_SRC = \
			  $(wildcard sources/library/io/*.c) \
			  $(wildcard sources/library/mem/*.c) \
			  $(wildcard sources/library/text/*.c) \
			  $(wildcard sources/library/task/lock.c) \
			  $(wildcard sources/library/ds/*.c) \
			  $(wildcard sources/library/log/*.c) \
			  $(wildcard sources/host/kernel/*.c) \
			  $(wildcard sources/library/alloc/*.c)

KERNEL_OBJ= \
	$(patsubst sources/%.c, $(CROSS_BUILDDIR)/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst sources/%.c, $(CROSS_BUILDDIR)/kernel/%.c.o, $(KERNEL_LIB_SRC)) \
	$(patsubst sources/%.s, $(CROSS_BUILDDIR)/%.s.o, $(KERNEL_S_SRC))

KERNEL_BIN=$(CROSS_BUILDDIR)/kernel.elf

TARGETS += $(KERNEL_BIN)

$(CROSS_BUILDDIR)/kernel/%.c.o: sources/kernel/%.c
	$(MKCWD)
	$(CROSS_CC) $(CROSS_KCFLAGS) -c -o $@ $^

$(CROSS_BUILDDIR)/kernel/library/%.c.o: sources/library/%.c
	$(MKCWD)
	$(CROSS_CC) $(CROSS_KCFLAGS) -c -o $@ $^

$(CROSS_BUILDDIR)/kernel/host/%.c.o: sources/host/%.c
	$(MKCWD)
	$(CROSS_CC) $(CROSS_KCFLAGS) -c -o $@ $^

$(KERNEL_BIN): $(KERNEL_OBJ)
	$(MKCWD)
	$(CROSS_LD) $(CROSS_KLDFLAGS) $^ -o $@
