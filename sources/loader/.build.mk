ifeq ($(CONFIG_TOOLCHAIN), llvm)

LOADER_CC=clang$(LLVM_VERSION)
ifeq (, $(shell which $(LOADER_CC) 2> /dev/null))
	LOADER_CC=clang
endif

LOADER_LD=clang$(LLVM_VERSION)
ifeq (, $(shell which $(LOADER_LD) 2> /dev/null))
	LOADER_LD=clang
endif

BINDIR_LOADER=bin/loader

LOADER_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(USER_CFLAGS_INC) \
	-ffreestanding \
	-D__x86_64__ \
	-D__efi__ \
	-D__loader__ \
	-target x86_64-unknown-windows \
	-fno-stack-protector \
	-fshort-wchar \
	-mno-red-zone

LOADER_LDFLAGS= \
	-target x86_64-unknown-windows \
	-nostdlib \
	-Wl,-entry:efi_main \
	-Wl,-subsystem:efi_application \
	-fuse-ld=lld-link

LOADER_SRC = \
	sources/libs/brutal/io/buf.c                    \
	sources/libs/brutal/io/fmt.c                    \
	sources/libs/brutal/io/file.c                   \
	sources/libs/brutal/io/std.c                    \
	sources/libs/brutal/io/print.c                  \
	sources/libs/brutal/parse/scan.c                \
	sources/libs/brutal/parse/nums.c                \
	sources/libs/brutal/parse/lex.c                 \
	sources/libs/brutal/io/write.c                  \
	sources/libs/stdc/ansi/string.c			        \
	sources/libs/stdc/ansi/ctype.c			        \
	$(wildcard sources/loader/*.c) 					\
	$(wildcard sources/libs/json/parser.c)		    \
	$(wildcard sources/libs/json/objects.c)		    \
	$(wildcard sources/libs/brutal/alloc/*.c)	    \
	$(wildcard sources/libs/brutal/ds/*.c) 		    \
	$(wildcard sources/libs/brutal/hash/*.c)	    \
	$(wildcard sources/embed/efi/*.c)	            \
	$(wildcard sources/embed/$(CONFIG_ARCH)/*.c)    \
	$(wildcard sources/libs/brutal/debug/*.c)       \
	$(wildcard sources/libs/brutal/mem/*.c)         \
	$(wildcard sources/libs/brutal/text/*.c)        \
	$(wildcard sources/libs/bal/boot/handover.c)    \
	$(wildcard sources/libs/elf/elf.c) 				\
	$(wildcard sources/libs/hw/efi/*/*.c)           \
	$(wildcard sources/libs/hw/efi/*.c)

LOADER_OBJS = \
	$(patsubst sources/%, $(BINDIR_LOADER)/%.o, $(LOADER_SRC)) \

LOADER=$(BINDIR_LOADER)/BOOTX64.EFI

ALL+=$(BINDIR_LOADER)/BOOTX64.EFI

$(LOADER): $(LOADER_OBJS)
	@$(MKCWD)
	$(LOADER_LD) -o $@ $^ $(LOADER_LDFLAGS)

$(BINDIR_LOADER)/%.c.o: sources/%.c
	@$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

$(BINDIR_LOADER)/libs/%.c.o: sources/libs/%.c
	@$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

$(BINDIR_LOADER)/tools/OVMF.fd:
	$(MKCWD)
	wget https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd
	mv DEBUGX64_OVMF.fd $@

run: $(LOADER) $(PKGS) $(KERNEL) $(BINDIR_LOADER)/tools/OVMF.fd
	$(MKCWD)

	cp -R sysroot/* $(BINDIR_LOADER)/image
	cp $(PKGS) $(BINDIR_LOADER)/image/pkgs

	mkdir -p $(BINDIR_LOADER)/image/EFI/BOOT/
	cp $(KERNEL) $(BINDIR_LOADER)/image/boot/kernel.elf
	cp $(LOADER) $(BINDIR_LOADER)/image/EFI/BOOT/BOOTX64.EFI

	qemu-system-x86_64 \
		$(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-no-reboot -no-shutdown\
		-bios $(BINDIR_LOADER)/tools/OVMF.fd \
		-drive file=fat:rw:$(BINDIR_LOADER)/image,media=disk,format=raw

endif
