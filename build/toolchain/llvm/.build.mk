CFLAGS_WARN += \
	-Wnewline-eof \
	-Wignored-attributes \
	-Wunknown-attributes

# --- Host compiler ---------------------------------------------------------- #

LLVM_VERSION ?=-12

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
	`pkg-config sdl2 --cflags`

HOST_LD=ld.lld
HOST_LDFLAGS= \
	`pkg-config sdl2 --libs`

HOST_AR=llvm-ar$(LLVM_VERSION)
ifeq (, $(shell which $(HOST_AR) 2> /dev/null))
	HOST_AR=llvm-ar
endif

HOST_ARFLAGS=rcs

USER_AS?=$(HOST_CC)

# --- User-Compiler --------------------------------------------------------- #

USER_CC=clang$(LLVM_VERSION) -target $(CONFIG_ARCH)-none-elf
ifeq (, $(shell which clang$(LLVM_VERSION) 2> /dev/null))
	USER_CC=clang -target $(CONFIG_ARCH)-none-elf
endif

USER_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(ARCH_CFLAGS) \
	$(USER_CFLAGS_INC) \
	-ffreestanding

USER_KCFLAGS= \
	$(USER_CFLAGS) \
	$(ARCH_KCFLAGS) \
	-D__kernel__=1

USER_UCFLAGS= \
	$(USER_CFLAGS) \
	-nostdlib \
	-D__brutal__=1

USER_LD=ld.lld
USER_KLDFLAGS= \
	-Tsources/embed/kernel/$(CONFIG_ARCH)/$(CONFIG_BOARD)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS)

USER_ULDFLAGS= \
	-Tsources/embed/brutal/$(CONFIG_ARCH)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS)

USER_OBJCOPY=llvm-objcopy$(LLVM_VERSION)
ifeq (, $(shell which $(USER_OBJCOPY) 2> /dev/null))
	USER_OBJCOPY=llvm-objcopy
endif

USER_AR?=llvm-ar$(LLVM_VERSION)
ifeq (, $(shell which $(USER_AR) 2> /dev/null))
	USER_AR=llvm-ar
endif

USER_ARFLAGS=rcs

ifeq ($(COVERAGE), yes)
	HOST_CFLAGS += \
		-fprofile-instr-generate -fcoverage-mapping

	HOST_LDFLAGS += \
		-fprofile-instr-generate -fcoverage-mapping

endif
