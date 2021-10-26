CFLAGS_WARN += \
	-Wnewline-eof \
	-Wignored-attributes \
	-Wunknown-attributes

# --- Host compiler ---------------------------------------------------------- #

LLVM_VERSION ?=-12
LLVM_CONFIG?=llvm-config$(LLVM_VERSION)

HOST_CC=clang$(LLVM_VERSION)
ifeq (, $(shell which $(HOST_CC) 2> /dev/null))
	HOST_CC=clang
endif

HOST_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(HOST_CFLAGS_INC) \
	-fsanitize=address \
	-fsanitize=undefined \
	`pkg-config sdl2 --cflags` \
	$(shell $(LLVM_CONFIG) --cflags)

HOST_LD=ld.lld
HOST_LDFLAGS=`pkg-config sdl2 --libs` $(shell $(LLVM_CONFIG) --ldflags)  $(shell $(LLVM_CONFIG) --libs)

HOST_AR=llvm-ar$(LLVM_VERSION)
ifeq (, $(shell which $(HOST_AR) 2> /dev/null))
	HOST_AR=llvm-ar
endif

HOST_ARFLAGS=rcs

# --- Cross-Compiler --------------------------------------------------------- #

CROSS_CC=clang$(LLVM_VERSION) -target $(CONFIG_ARCH)-none-elf
ifeq (, $(shell which clang$(LLVM_VERSION) 2> /dev/null))
	CROSS_CC=clang -target $(CONFIG_ARCH)-none-elf
endif

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

CROSS_AR?=llvm-ar$(LLVM_VERSION)
ifeq (, $(shell which $(CROSS_AR) 2> /dev/null))
	CROSS_AR=llvm-ar
endif

CROSS_ARFLAGS=rcs
