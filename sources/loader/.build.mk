LOADER_CC = clang-12
LOADER_LD = clang-12

BINDIR_LOADER=bin/loader

LOADER_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(CROSS_CFLAGS_INC) \
	-ffreestanding \
	-D__x86_64__ \
	-D__efi__ \
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

LOADER_LIBS_SRC = \
	sources/libs/brutal/io/buffer.c                 \
	sources/libs/brutal/io/fmt.c                    \
	sources/libs/brutal/io/file.c                   \
	sources/libs/brutal/io/std.c                    \
	sources/libs/brutal/io/print.c                  \
	sources/libs/brutal/parse/scan.c                \
	sources/libs/brutal/parse/lex.c                 \
	sources/libs/brutal/io/write.c                  \
	sources/libs/stdc/ansi/string.c			        \
	sources/libs/stdc/ansi/ctype.c			        \
	$(wildcard sources/libs/json/*.c)		        \
	$(wildcard sources/libs/brutal/alloc/*.c)	    \
	$(wildcard sources/libs/brutal/ds/*.c) 		    \
	$(wildcard sources/libs/brutal/hash/*.c)	    \
	$(wildcard sources/host/efi/*.c)	            \
	$(wildcard sources/libs/brutal/log/*.c)         \
	$(wildcard sources/libs/brutal/mem/*.c)         \
	$(wildcard sources/libs/brutal/text/*.c)        \
	$(wildcard sources/libs/bal/abi/handover.c) \
	$(wildcard sources/libs/elf/elf.c) 				\
	$(wildcard sources/libs/hw/efi/*/*.c)              \
	$(wildcard sources/libs/hw/efi/*.c)

LOADER_SRCS = $(wildcard sources/loader/*.c)

LOADER_OBJS = \
	$(patsubst sources/%.c, $(BINDIR_LOADER)/%.c.o, $(LOADER_SRCS)) \
	$(patsubst sources/%.c, $(BINDIR_LOADER)/%.c.o, $(LOADER_LIBS_SRC))

LOADER=$(BINDIR_LOADER)/BOOTX64.EFI

$(LOADER): $(LOADER_OBJS)
	$(MKCWD)
	$(LOADER_LD) -o $@ $^ $(LOADER_LDFLAGS)

$(BINDIR_LOADER)/%.c.o: sources/%.c
	$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

$(BINDIR_LOADER)/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

$(BINDIR_LOADER)/tools/OVMF.fd:
	$(MKCWD)
	wget https://efi.akeo.ie/OVMF/OVMF-X64.zip
	unzip OVMF-X64.zip OVMF.fd
	rm OVMF-X64.zip
	mv OVMF.fd $@

loader: $(LOADER)

run-loader: $(LOADER) $(KERNEL) $(BINDIR_LOADER)/tools/OVMF.fd

	mkdir -p $(BINDIR_LOADER)/image/EFI/BOOT/
	cp $(KERNEL) $(BINDIR_LOADER)/image/kernel.elf
	cp sources/loader/config.json $(BINDIR_LOADER)/image/config.json
	cp $(LOADER) $(BINDIR_LOADER)/image/EFI/BOOT/BOOTX64.EFI

	qemu-system-x86_64 \
		-serial stdio \
		-m 256 \
		-bios $(BINDIR_LOADER)/tools/OVMF.fd \
		-drive file=fat:rw:$(BINDIR_LOADER)/image,media=disk,format=raw
