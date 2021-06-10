KERNEL_C_SRC+=$(wildcard sources/kernel/*.c) \
			  $(wildcard sources/kernel/*/*.c)

KERNEL_LIB_SRC = \
			  $(wildcard sources/library/io/*.c) \
			  $(wildcard sources/library/mem/*.c) \
			  $(wildcard sources/library/text/*.c) \
			  $(wildcard sources/library/task/lock.c) \
			  $(wildcard sources/library/ds/*.c) \
			  $(wildcard sources/library/log/*.c) \
			  $(wildcard sources/libc/ansi/string.c) \
			  $(wildcard sources/host/kernel/*.c) \
			  $(wildcard sources/library/alloc/*.c)

KERNEL_OBJ= \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/kernel/%.c.o, $(KERNEL_LIB_SRC)) \
	$(patsubst sources/%.s, $(BUILDDIR_CROSS)/%.s.o, $(KERNEL_S_SRC))

KERNEL_BIN=$(BUILDDIR_CROSS)/kernel.elf

TARGETS += $(KERNEL_BIN)
DEPENDENCIES += $(KERNEL_OBJ:.o=.d)

$(BUILDDIR_CROSS)/kernel/%.c.o: sources/kernel/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/kernel/library/%.c.o: sources/library/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/kernel/libc/%.c.o: sources/libc/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_CROSS)/kernel/host/%.c.o: sources/host/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(KERNEL_BIN): $(KERNEL_OBJ)
	$(MKCWD)
	$(CROSS_LD) -o $@ $^ $(CROSS_KLDFLAGS)
