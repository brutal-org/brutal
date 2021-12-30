BINDIR_KERNEL = $(BINDIR)/kernel

KERNEL=$(BINDIR_KERNEL)/kernel.elf

KERNEL_SRC += \
	$(wildcard sources/kernel/*.c)                 \
	$(wildcard sources/kernel/$(CONFIG_ARCH)/*.c)  \
	$(wildcard sources/kernel/$(CONFIG_ARCH)/*.s)  \
	$(wildcard sources/embed/arch/$(CONFIG_ARCH)/*.c)   \
	$(wildcard sources/embed/arch/$(CONFIG_ARCH)/*.s)   \
	$(wildcard sources/embed/kernel/*.c)           \
	sources/libs/bal/abi/helpers.c                 \
	sources/libs/brutal/io/fmt.c                   \
	sources/libs/brutal/io/print.c                 \
	sources/libs/brutal/io/emit.c                  \
	sources/libs/brutal/io/write.c                 \
	sources/libs/brutal/time/convert.c             \
	sources/libs/elf/elf.c                         \
	sources/libs/bal/boot/handover.c               \
	sources/libs/stdc/ansi/ctype.c                 \
	sources/libs/stdc/ansi/string.c                \
	sources/libs/brutal/io/buf.c                   \
	$(wildcard sources/libs/hw/acpi/*.c)           \
	$(wildcard sources/libs/brutal/alloc/*.c)      \
	$(wildcard sources/libs/brutal/ds/*.c)         \
	$(wildcard sources/libs/brutal/hash/*.c)       \
	$(wildcard sources/libs/hw/fdt/*.c)            \
	$(wildcard sources/libs/hw/uart/*.c)           \
	$(wildcard sources/libs/brutal/parse/*.c)      \
	$(wildcard sources/libs/brutal/debug/*.c)      \
	$(wildcard sources/libs/brutal/mem/*.c)        \
	$(wildcard sources/libs/brutal/sync/*.c)       \
	$(wildcard sources/libs/brutal/text/*.c)       \
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
