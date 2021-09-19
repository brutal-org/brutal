export PATH := $(shell meta/toolchain/gnu/use.sh):$(PATH)

CFLAGS_WARN+=-Wc++-compat

# --- Host compiler ---------------------------------------------------------- #

HOST_CC=cc

HOST_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(HOST_CFLAGS_INC) \
	-fsanitize=address \
	-fsanitize=undefined \
	`pkg-config  sdl2 --cflags`

HOST_LD=ld
HOST_LDFLAGS=`pkg-config  sdl2 --libs`

HOST_AR=ar
HOST_ARFLAGS=rcs

# --- Cross-Compiler --------------------------------------------------------- #

CROSS_AS=nasm
CROSS_ASFLAGS=-f elf64

CROSS_CC=$(CONFIG_ARCH)-elf-gcc

CROSS_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(CROSS_CFLAGS_INC) \
	-fsanitize=undefined \
	-ffreestanding \
	-D__brutal__=1

CROSS_KCFLAGS= \
	$(CROSS_CFLAGS) \
	$(ARCH_KCFLAGS) \
	-fno-stack-protector \
	-D__kernel__=1

CROSS_UCFLAGS= \
	$(CROSS_CFLAGS) \
	-nostdlib

CROSS_LD=$(CONFIG_ARCH)-elf-ld
CROSS_KLDFLAGS= \
	-Tsources/host/kernel/$(CONFIG_ARCH)/link.ld \
	-z max-page-size=0x1000 \
	meta/toolchain/gnu/local/lib/gcc/x86_64-elf/11.1.0/libgcc.a

CROSS_ULDFLAGS= \
	-Tsources/host/brutal/$(CONFIG_ARCH)/link.ld \
	-z max-page-size=0x1000 \
	meta/toolchain/gnu/local/lib/gcc/x86_64-elf/11.1.0/libgcc.a

CROSS_AR=$(CONFIG_ARCH)-elf-ar
CROSS_ARFLAGS=rcs
