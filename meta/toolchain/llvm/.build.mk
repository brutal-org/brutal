CFLAGS_WARN += \
	-Wnewline-eof \
	-Wignored-attributes \
	-Wunknown-attributes

# --- Host compiler ---------------------------------------------------------- #

CLANG?=clang-12

HOST_CC=$(CLANG)

HOST_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(HOST_CFLAGS_INC) \
	-fsanitize=address \
	-fsanitize=undefined \
	`pkg-config sdl2 --cflags`

HOST_LD=ld.lld
HOST_LDFLAGS=`pkg-config sdl2 --libs`

HOST_AR=ar
HOST_ARFLAGS=rcs

# --- Cross-Compiler --------------------------------------------------------- #

CROSS_CC=$(CLANG) -target $(CONFIG_ARCH)-none-elf
CROSS_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(CROSS_CFLAGS_INC) \
	-ffreestanding

CROSS_KCFLAGS= \
	$(CROSS_CFLAGS) \
	$(ARCH_KCFLAGS) \
	-D__kernel__=1

CROSS_UCFLAGS= \
	$(CROSS_CFLAGS) \
	-nostdlib \
	-D__brutal__=1

CROSS_LD=ld.lld
CROSS_KLDFLAGS= \
	-Tsources/host/kernel/$(CONFIG_ARCH)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS)

CROSS_ULDFLAGS= \
	-Tsources/host/brutal/$(CONFIG_ARCH)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS)

CROSS_AR=ar
CROSS_ARFLAGS=rcs
