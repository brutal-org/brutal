BINDIR_KERNEL = $(BINDIR)/kernel

KERNEL=$(BINDIR_KERNEL)/kernel.elf

KERNEL_SRC += \
	$(wildcard sources/kernel/*.c)                 \
	$(wildcard sources/kernel/$(CONFIG_ARCH)/*.c)  \
	$(wildcard sources/kernel/$(CONFIG_ARCH)/*.s)  \
	$(wildcard sources/embed/$(CONFIG_ARCH)/*.c)   \
	$(wildcard sources/embed/$(CONFIG_ARCH)/*.s)   \
	$(wildcard sources/embed/kernel/*.c)           \
	$(wildcard sources/libs/stdc/ansi/ctype.c)     \
	$(wildcard sources/libs/stdc/ansi/string.c)    \
	$(wildcard sources/libs/hw/acpi/*.c)           \
	$(wildcard sources/libs/brutal/alloc/*.c)      \
	$(wildcard sources/libs/brutal/ds/*.c)         \
	$(wildcard sources/libs/brutal/hash/*.c)       \
	$(wildcard sources/libs/brutal/io/buf.c)       \
	$(wildcard sources/libs/brutal/io/fmt.c)       \
	$(wildcard sources/libs/brutal/io/print.c)     \
	$(wildcard sources/libs/brutal/parse/*.c)      \
	$(wildcard sources/libs/brutal/io/write.c)     \
	$(wildcard sources/libs/brutal/debug/*.c)      \
	$(wildcard sources/libs/brutal/mem/*.c)        \
	$(wildcard sources/libs/brutal/sync/*.c)       \
	$(wildcard sources/libs/brutal/text/*.c)       \
	$(wildcard sources/libs/brutal/time/convert.c) \
	$(wildcard sources/libs/elf/elf.c)             \
	$(wildcard sources/libs/bal/boot/handover.c)    \
	$(wildcard sources/libs/ubsan/*.c)

list-kernel-src:
	@echo $(KERNEL_SRC)

KERNEL_OBJ= \
	$(patsubst sources/%, $(BINDIR_KERNEL)/%.o, $(KERNEL_SRC))

DEPENDENCIES += $(KERNEL_OBJ:.o=.d)

$(BINDIR_KERNEL)/%.c.o: sources/%.c
	@$(MKCWD)
	$(USER_CC) -c -o $@ $< $(USER_KCFLAGS)

$(BINDIR_KERNEL)/%.s.o: sources/%.s
	@$(MKCWD)
	$(USER_AS) -o $@ $< $(USER_ASFLAGS)

$(KERNEL): $(KERNEL_OBJ)
	@$(MKCWD)
	$(USER_LD) -o $@ $^ $(USER_KLDFLAGS)

ALL+=$(KERNEL)
