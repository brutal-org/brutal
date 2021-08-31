CFLAGS_WARN += \
	-Wnewline-eof \
	-Wignored-attributes \
	-Wunknown-attributes

# --- Host compiler ---------------------------------------------------------- #

HOST_CC=clang-12

HOST_CFLAGS= \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(HOST_CFLAGS_INC)

HOST_LD=ld
HOST_LDFLAGS=

HOST_AR=ar
HOST_ARFLAGS=rcsv

# --- Cross-Compiler --------------------------------------------------------- #

CROSS_AS=nasm
CROSS_ASFLAGS=-f elf64

CROSS_CC=clang-12 -target $(CONFIG_ARCH)-none-elf
CROSS_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(CROSS_CFLAGS_INC) \
	-ffreestanding

CROSS_KCFLAGS= \
	$(CROSS_CFLAGS) \
	-fsanitize=undefined \
	-fno-stack-protector \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel \
	-D__kernel__=1

CROSS_UCFLAGS= \
	$(CROSS_CFLAGS) \
	-nostdlib \
	-D__brutal__=1

CROSS_LD=ld.lld
CROSS_KLDFLAGS= \
	-Tsources/archs/$(CONFIG_ARCH)/kernel.ld \
	-z max-page-size=0x1000

CROSS_ULDFLAGS= \
	-Tsources/archs/$(CONFIG_ARCH)/userspace.ld \
	-z max-page-size=0x1000

CROSS_AR=ar
CROSS_ARFLAGS=rcsv
