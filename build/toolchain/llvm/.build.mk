CFLAGS_WARN += \
	-Wnewline-eof \
	-Wignored-attributes \
	-Wunknown-attributes

# --- Host compiler ---------------------------------------------------------- #

LLVM_VERSION ?=-12

HOST_CC=clang$(LLVM_VERSION)

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

HOST_AR=llvm-ar$(LLVM_VERSION)
HOST_ARFLAGS=rcs

# --- Cross-Compiler --------------------------------------------------------- #

CROSS_CC=clang$(LLVM_VERSION) -target $(CONFIG_ARCH)-none-elf
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
	-Tsources/embed/kernel/$(CONFIG_ARCH)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS)

CROSS_ULDFLAGS= \
	-Tsources/embed/brutal/$(CONFIG_ARCH)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS)

CROSS_AR=llvm-ar$(LLVM_VERSION)
CROSS_ARFLAGS=rcs