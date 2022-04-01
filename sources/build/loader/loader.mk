ifeq ($(TOOLCHAIN), llvm)

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
	sources/libs/bal/boot/handover.c                \
	sources/libs/bruwutal/io/buf.c                    \
	sources/libs/bruwutal/io/emit.c                   \
	sources/libs/bruwutal/io/file.c                   \
	sources/libs/bruwutal/io/fmt.c                    \
	sources/libs/bruwutal/io/funcs.c                  \
	sources/libs/bruwutal/io/base.c                   \
	sources/libs/bruwutal/io/std.c                    \
	sources/libs/bruwutal/parse/lex.c                 \
	sources/libs/bruwutal/parse/nums.c                \
	sources/libs/bruwutal/parse/scan.c                \
	sources/libs/hw/fdt/fdt.c                       \
	sources/libs/stdc-ansi/ctype.c			        \
	sources/libs/stdc-ansi/string.c			        \
	$(wildcard sources/loader/*.c) 					\
	$(wildcard sources/libs/json/parser.c)		    \
	$(wildcard sources/libs/json/objects.c)		    \
	$(wildcard sources/libs/bruwutal/alloc/*.c)	    \
	$(wildcard sources/libs/bruwutal/ds/*.c) 		    \
	$(wildcard sources/libs/bruwutal/hash/*.c)	    \
	$(wildcard sources/libs/embed/efi/*.c)	            \
	$(wildcard sources/libs/embed/$(ARCH)/*.c) \
	$(wildcard sources/libs/bruwutal/debug/*.c)       \
	$(wildcard sources/libs/bruwutal/mem/*.c)         \
	$(wildcard sources/libs/bruwutal/text/*.c)        \
	$(wildcard sources/libs/elf/elf.c) 				\
	$(wildcard sources/libs/hw/efi/*/*.c)           \
	$(wildcard sources/libs/hw/efi/*.c)

LOADER_OBJS = \
	$(patsubst sources/%, $(BINDIR_LOADER)/%.o, $(LOADER_SRC)) \

LOADER=$(BINDIR_LOADER)/BOOTX64.EFI

ALL+=$(LOADER)

$(LOADER): $(LOADER_OBJS)
	@$(MKCWD)
	$(LOADER_LD) -o $@ $^ $(LOADER_LDFLAGS)

$(BINDIR_LOADER)/%.c.o: sources/%.c
	@$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

$(BINDIR_LOADER)/libs/%.c.o: sources/libs/%.c
	@$(MKCWD)
	$(LOADER_CC) -c -o $@ $< $(LOADER_CFLAGS)

endif
