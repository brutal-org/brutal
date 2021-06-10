export PATH := $(shell meta/toolchain/gnu/use.sh):$(PATH)

# --- Host compiler ---------------------------------------------------------- #

HOST_CC=cc

HOST_CFLAGS= \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(CFLAGS_INC)

HOST_LD=ld
HOST_LDFLAGS=

# --- Cross-Compiler --------------------------------------------------------- #

CROSS_AS=nasm
CROSS_ASFLAGS=-f elf64

CROSS_CC=x86_64-elf-gcc
CROSS_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(CFLAGS_INC) \
	-ffreestanding

CROSS_KCFLAGS= \
	$(CROSS_CFLAGS) \
	-fno-pic \
	-fpie \
	-fno-stack-protector \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone

CROSS_LD=x86_64-elf-ld
CROSS_KLDFLAGS= \
	-Tsources/arch/x86_64/link.ld \
	-z max-page-size=0x1000

CROSS_AR=x86_64-elf-ar
CROSS_ARFLAGS=rcsv
