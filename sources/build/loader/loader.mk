LOADER_CC=clang$(LLVM_VERSION)
ifeq (, $(shell which $(LOADER_CC) 2> /dev/null))
	LOADER_CC=clang
endif

LOADER_LD=clang$(LLVM_VERSION)
ifeq (, $(shell which $(LOADER_LD) 2> /dev/null))
	LOADER_LD=clang
endif

LOADER_CFLAGS= \
	$(BASE_CFLAGS) \
	$(USER_CFLAGS_INC) \
	-ffreestanding \
	-D__x86_64__ \
	-D__efi__ \
	-D__freestanding__ \
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
	sources/libs/bal/boot/handover.c \
	sources/libs/brutal-io/buf.c \
	sources/libs/brutal-io/emit.c \
	sources/libs/brutal-io/file.c \
	sources/libs/brutal-io/funcs.c \
	sources/libs/brutal-io/base.c \
	sources/libs/brutal-io/chan.c \
	sources/libs/brutal-parse/lex.c \
	sources/libs/brutal-parse/nums.c \
	sources/libs/brutal-parse/scan.c \
	sources/libs/hw/fdt/fdt.c \
	sources/libs/stdc-ansi/ctype.c \
	sources/libs/stdc-ansi/string.c \
	$(wildcard sources/loader/*.c) \
	$(wildcard sources/libs/json/parser.c) \
	$(wildcard sources/libs/json/objects.c) \
	$(wildcard sources/libs/brutal-alloc/*.c) \
	$(wildcard sources/libs/brutal-ds/*.c) \
	$(wildcard sources/libs/brutal-hash/*.c) \
	$(wildcard sources/libs/embed/efi/*.c) \
	$(wildcard sources/libs/embed/$(ARCH)/*.c) \
	$(wildcard sources/libs/brutal-debug/*.c) \
	$(wildcard sources/libs/brutal-mem/*.c) \
	$(wildcard sources/libs/brutal-text/*.c) \
	$(wildcard sources/libs/brutal-fmt/*.c) \
	$(wildcard sources/libs/elf/elf.c) \
	$(wildcard sources/libs/hw/efi/*/*.c) \
	$(wildcard sources/libs/hw/efi/*.c)

LOADER_OBJS = \
	$(patsubst sources/%, $(BINDIR_LOADER)/%.o, $(LOADER_SRC)) \

LOADER=$(BINDIR_LOADER)/BOOTX64.EFI

ALL+=$(LOADER)

$(LOADER): $(LOADER_OBJS)
	@$(MKCWD)
	$(LOADER_LD) -o $@ $^ $(LOADER_LDFLAGS)

$(BINDIR_LOADER)/%.c.o: sources/%.c | $(GENERATED_MOD)
	@$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

$(BINDIR_LOADER)/libs/%.c.o: sources/libs/%.c | $(GENERATED_MOD)
	@$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)
