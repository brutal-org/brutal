BASE_CFLAGS += \
	-Wnewline-eof \
	-Wignored-attributes \
	-Wunknown-attributes

# --- Host compiler ---------------------------------------------------------- #

LLVM_VERSION ?=-13

HOST_CC=clang$(LLVM_VERSION)
ifeq (, $(shell which $(HOST_CC) 2> /dev/null))
	HOST_CC=clang
endif

HOST_CFLAGS= \
	$(BASE_CFLAGS) \
	`pkg-config sdl2 --cflags`

HOST_LD=ld.lld$(LLVM_VERSION)
ifeq (, $(shell which $(HOST_LD) 2> /dev/null))
	HOST_LD=ld.lld
endif

HOST_LDFLAGS= \
	`pkg-config sdl2 --libs` \
	-lm

HOST_AR=llvm-ar$(LLVM_VERSION)
ifeq (, $(shell which $(HOST_AR) 2> /dev/null))
	HOST_AR=llvm-ar
endif

HOST_ARFLAGS=rcs

USER_AS?=$(HOST_CC)
HOST_AS?=$(HOST_CC)

# --- User-Compiler --------------------------------------------------------- #

USER_CC=clang$(LLVM_VERSION) -target $(ARCH)-none-elf
ifeq (, $(shell which clang$(LLVM_VERSION) 2> /dev/null))
	USER_CC=clang -target $(ARCH)-none-elf
endif

USER_CFLAGS= \
	$(BASE_CFLAGS) \
	$(ARCH_CFLAGS) \
	$(USER_CFLAGS_INC) \
	-D__brutal__=1 \
	-ffreestanding \
	-Wimplicit-fallthrough

USER_KCFLAGS= \
	$(BASE_CFLAGS) \
	$(ARCH_KCFLAGS) \
	$(USER_CFLAGS_INC) \
	-ffreestanding \
	-fno-stack-protector \
	-D__brutal__=1 \
	-D__kernel__=1 \
	-D__freestanding__=1 \
	-Wimplicit-fallthrough

USER_UCFLAGS= \
	$(USER_CFLAGS) \
	-nostdlib

USER_LD=ld.lld$(LLVM_VERSION)
ifeq (, $(shell which $(USER_LD) 2> /dev/null))
	USER_LD=ld.lld
endif

USER_KLDFLAGS= \
	-Tsources/build/boards/$(ARCH)-$(BOARD)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS) \
	$(ARCH_KLDFLAGS)

USER_ULDFLAGS= \
	-Tsources/build/target/$(ARCH)-link.ld \
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
		-fprofile-instr-generate \
		-fcoverage-mapping

	HOST_LDFLAGS += \
		-fprofile-instr-generate \
		-fcoverage-mapping
endif
