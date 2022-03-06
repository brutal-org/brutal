export PATH := $(shell build/toolchain/gnu/use.sh):$(PATH)

# --- Host compiler ---------------------------------------------------------- #

HOST_CC=gcc

HOST_CFLAGS= \
	$(BASE_CFLAGS) \
	`pkg-config sdl2 --cflags` \
	-fanalyzer

HOST_LD=ld
HOST_LDFLAGS= \
	-lm \
	`pkg-config sdl2 --libs`

HOST_AR=ar
HOST_ARFLAGS=rcs

# default can be changed in archs

USER_AS?=$(ARCH)-elf-as
HOST_AS?=$(ARCH)-elf-as

# --- User-Compiler ---------------------------------------------------------- #

USER_CC=$(ARCH)-elf-gcc

USER_CFLAGS= \
	$(BASE_CFLAGS) \
	$(ARCH_CFLAGS) \
	$(USER_CFLAGS_INC) \
	-D__brutal__=1 \
	-ffreestanding

USER_KCFLAGS= \
	$(BASE_CFLAGS) \
	$(ARCH_KCFLAGS) \
	$(USER_CFLAGS_INC) \
	-fanalyzer \
	-ffreestanding \
	-fno-stack-protector \
	-D__brutal__=1 \
	-D__kernel__=1 \
	-D__freestanding__=1

USER_UCFLAGS= \
	$(USER_CFLAGS) \
	-fanalyzer \
	-nostdlib

USER_LD=$(ARCH)-elf-ld
USER_KLDFLAGS= \
	-Tsources/build/boards/$(ARCH)-$(BOARD)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS) \
	build/toolchain/gnu/local/lib/gcc/$(ARCH)-elf/11.1.0/libgcc.a

USER_ULDFLAGS= \
	-Tsources/build/target/$(ARCH)-link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS) \
	build/toolchain/gnu/local/lib/gcc/$(ARCH)-elf/11.1.0/libgcc.a

USER_OBJCOPY=$(ARCH)-elf-objcopy

USER_AR=$(ARCH)-elf-ar
USER_ARFLAGS=rcs

ifeq ($(COVERAGE), yes)
	HOST_CFLAGS += \
		-coverage -g

	HOST_LDFLAGS += \
		-coverage
endif
