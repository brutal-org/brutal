CC = clang
LD = lld-link

CFLAGS = $(CROSS_CFLAGS) -I. -target x86_64-pc-win32-coff -fno-stack-protector -fshort-wchar -mno-red-zone

LDFLAGS = -subsystem:efi_application -nodefaultlib -dll

DIRECTORY_GUARD=@mkdir -p $(@D)

BRUTAL_BOOT_LIBS_SRCS = \
	sources/libs/brutal/io/fmt.c \
	sources/libs/brutal/io/write.c \
	sources/libs/brutal/io/print.c \
	sources/libs/brutal/io/scan.c \
	$(wildcard sources/libs/brutal/log/*.c) \
	$(wildcard sources/libs/brutal/alloc/*.c)	\
	$(wildcard sources/libs/elf/elf.c)              \
	$(wildcard sources/libs/efi/*.c)                \
	$(wildcard sources/libs/ansi/ctype.c)           \
	$(wildcard sources/libs/ansi/string.c)          \
	$(wildcard sources/libs/brutal/mem/*.c)         \
	$(wildcard sources/libs/brutal/text/*.c)        \

BRUTAL_BOOT_SRCS = $(wildcard sources/loader/src/*.c)

OBJS = \
	$(patsubst sources/%.c, $(BUILD_DIRECTORY)/%.c.o, $(BRUTAL_BOOT_SRCS)) \
	$(patsubst sources/%.c, $(BUILD_DIRECTORY)/%.c.o, $(BRUTAL_BOOT_LIBS_SRCS)) \

BUILD_DIRECTORY = build/loader

EFI_FILE = $(BUILD_DIRECTORY)/main.efi

$(EFI_FILE): $(OBJS)
	$(LD) $(LDFLAGS) -entry:efi_main $^ -out:$@
	$(MAKE) -C sources/loader/test


$(BUILD_DIRECTORY)/%.c.o: sources/%.c
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIRECTORY)/libs/%.c.o: sources/libs/%.c
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) -c -o $@ $<

run-loader: $(EFI_FILE) $(BUILD_DIRECTORY)/tools/OVMF.fd $(BUILD_DIRECTORY)/image/EFI/BOOT/BOOTX64.EFI
	qemu-system-x86_64 -bios $(BUILD_DIRECTORY)/tools/OVMF.fd -machine q35 -serial stdio -cpu host -enable-kvm -drive file=fat:rw:$(BUILD_DIRECTORY)/image,media=disk,format=raw

$(BUILD_DIRECTORY)/image/EFI/BOOT/BOOTX64.EFI:
	$(DIRECTORY_GUARD)
	ln -sf ../../../../../$(EFI_FILE) $@

$(BUILD_DIRECTORY)/tools/OVMF.fd:
	$(DIRECTORY_GUARD)
	wget https://efi.akeo.ie/OVMF/OVMF-X64.zip
	unzip OVMF-X64.zip OVMF.fd
	rm OVMF-X64.zip
	mv OVMF.fd $(@D)

