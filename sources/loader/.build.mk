LOADER_CC = clang
LOADER_LD = lld-link

BUILDDIR_LOADER=build/loader

LOADER_CFLAGS= \
	$(CROSS_CFLAGS) \
	-D__x86_64__ \
	-D__efi__ \
	-target x86_64-pc-win32-coff \
	-fno-stack-protector \
	-fshort-wchar \
	-mno-red-zone

LOADER_LDFLAGS= \
	-entry:efi_main \
	-subsystem:efi_application \
	-nodefaultlib \
	-dll

LOADER_LIBS_SRC = \
	sources/libs/brutal/io/buffer.c                 \
	sources/libs/brutal/io/fmt.c                    \
	sources/libs/brutal/io/file.c                   \
	sources/libs/brutal/io/std.c                    \
	sources/libs/brutal/io/print.c                  \
	sources/libs/brutal/io/scan.c                   \
	sources/libs/brutal/io/write.c                  \
	sources/libs/ansi/string.c			\
	$(wildcard sources/libs/json/*.c)		\
	$(wildcard sources/libs/brutal/alloc/*.c)	\
	$(wildcard sources/libs/brutal/ds/*.c) 		\
	$(wildcard sources/libs/brutal/hash/*.c)	\
	$(wildcard sources/libs/brutal/host/efi/*.c)	\
	$(wildcard sources/libs/brutal/log/*.c)         \
	$(wildcard sources/libs/brutal/mem/*.c)         \
	$(wildcard sources/libs/brutal/text/*.c)        \
	$(wildcard sources/libs/ansi/ctype.c)           \
	$(wildcard sources/libs/ansi/string.c)          \
	$(wildcard sources/libs/handover/*.c)		    \
	$(wildcard sources/libs/elf/elf.c) 				\
	$(wildcard sources/libs/efi/*/*.c)              \
	$(wildcard sources/libs/efi/*.c)

LOADER_SRCS = $(wildcard sources/loader/*.c)

LOADER_OBJS = \
	$(patsubst sources/%.c, $(BUILDDIR_LOADER)/%.c.o, $(LOADER_SRCS)) \
	$(patsubst sources/%.c, $(BUILDDIR_LOADER)/%.c.o, $(LOADER_LIBS_SRC)) \

LOADER=$(BUILDDIR_CROSS)/BOOTX64.EFI

$(LOADER): $(LOADER_OBJS)
	$(MKCWD)
	$(LOADER_LD) -out:$@ $^  $(LOADER_LDFLAGS)

$(BUILDDIR_LOADER)/%.c.o: sources/%.c
	$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

$(BUILDDIR_LOADER)/libs/%.c.o: sources/libs/%.c
	$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

$(BUILDDIR_LOADER)/tools/OVMF.fd:
	$(MKCWD)
	wget https://efi.akeo.ie/OVMF/OVMF-X64.zip
	unzip OVMF-X64.zip OVMF.fd
	rm OVMF-X64.zip
	mv OVMF.fd $@

loader: $(LOADER)

run-loader: $(LOADER) $(BUILDDIR_LOADER)/tools/OVMF.fd
	$(MAKE) -C sources/loader/test clean
	$(MAKE) -C sources/loader/test

	mkdir -p $(BUILDDIR_LOADER)/image/EFI/BOOT/
	cp sources/loader/test/kernel.elf $(BUILDDIR_LOADER)/image/
	cp sources/loader/test/config.json $(BUILDDIR_LOADER)/image/
	cp $(LOADER) $(BUILDDIR_LOADER)/image/EFI/BOOT/BOOTX64.EFI

	qemu-system-x86_64 \
		-serial stdio \
		-m 256 \
		-bios $(BUILDDIR_LOADER)/tools/OVMF.fd \
		-cpu host \
		-enable-kvm \
		-drive file=fat:rw:$(BUILDDIR_LOADER)/image,media=disk,format=raw
