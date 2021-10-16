BUILDDIR_KERNEL = $(BUILDDIR)/kernel

KERNEL=$(BUILDDIR_KERNEL)/kernel.elf

KERNEL_SRC += \
	$(wildcard sources/kernel/*.c)                 \
	$(wildcard sources/kernel/$(CONFIG_ARCH)/*.c)  \
	$(wildcard sources/kernel/$(CONFIG_ARCH)/*.s)  \
	$(wildcard sources/libs/stdc/ansi/ctype.c)     \
	$(wildcard sources/libs/stdc/ansi/string.c)    \
	$(wildcard sources/libs/acpi/*.c)              \
	$(wildcard sources/libs/brutal/alloc/*.c)      \
	$(wildcard sources/libs/brutal/ds/*.c)         \
	$(wildcard sources/libs/brutal/hash/*.c)       \
	$(wildcard sources/host/kernel/*.c)            \
	$(wildcard sources/libs/brutal/io/buffer.c)    \
	$(wildcard sources/libs/brutal/io/fmt.c)       \
	$(wildcard sources/libs/brutal/io/print.c)     \
	$(wildcard sources/libs/brutal/parse/*.c)      \
	$(wildcard sources/libs/brutal/io/write.c)     \
	$(wildcard sources/libs/brutal/log/*.c)        \
	$(wildcard sources/libs/brutal/mem/*.c)        \
	$(wildcard sources/libs/brutal/sync/*.c)       \
	$(wildcard sources/libs/brutal/text/*.c)       \
	$(wildcard sources/libs/brutal/time/convert.c) \
	$(wildcard sources/libs/elf/elf.c)             \
	$(wildcard sources/libs/handover/*.c)          \
	$(wildcard sources/libs/ubsan/*.c)

KERNEL_OBJ= \
	$(patsubst sources/%, $(BUILDDIR_KERNEL)/%.o, $(KERNEL_SRC))

DEPENDENCIES += $(KERNEL_OBJ:.o=.d)

$(BUILDDIR_KERNEL)/%.c.o: sources/%.c
	$(ECHO) "\e[37mkernel \e[92mCC\e[m" $<
	@$(MKCWD)
	@$(CROSS_CC) -c -o $@ $< $(CROSS_KCFLAGS)

$(BUILDDIR_KERNEL)/%.s.o: sources/%.s
	$(ECHO) "\e[37mkernel \e[92mAS\e[m" $<
	@$(MKCWD)
	@$(CROSS_AS) -o $@ $< $(CROSS_ASFLAGS)

$(KERNEL): $(KERNEL_OBJ)
	$(ECHO) "\e[37mkernel \e[92mLD\e[m" $@
	@$(MKCWD)
	@$(CROSS_LD) -o $@ $^ $(CROSS_KLDFLAGS)
