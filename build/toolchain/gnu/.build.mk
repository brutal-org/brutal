export PATH := $(shell build/toolchain/gnu/use.sh):$(PATH)

# --- Host compiler ---------------------------------------------------------- #

HOST_CC=cc

HOST_CFLAGS= \
	-fanalyzer \
	$(BASE_CFLAGS) \
	`pkg-config sdl2 --cflags`

HOST_LD=ld
HOST_LDFLAGS= \
	-lm \
	`pkg-config sdl2 --libs`

HOST_AR=ar
HOST_ARFLAGS=rcs

# default can be changed in archs

USER_AS?=$(CONFIG_ARCH)-elf-as

# --- User-Compiler ---------------------------------------------------------- #
USER_CC=$(CONFIG_ARCH)-elf-gcc

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

USER_LD=$(CONFIG_ARCH)-elf-ld
USER_KLDFLAGS= \
	-Tsources/build/boards/$(CONFIG_ARCH)-$(CONFIG_BOARD)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS) \
	build/toolchain/gnu/local/lib/gcc/$(CONFIG_ARCH)-elf/11.1.0/libgcc.a

USER_ULDFLAGS= \
	-Tsources/build/target/$(CONFIG_ARCH)-link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS) \
	build/toolchain/gnu/local/lib/gcc/$(CONFIG_ARCH)-elf/11.1.0/libgcc.a

USER_OBJCOPY=$(CONFIG_ARCH)-elf-objcopy

USER_AR=$(CONFIG_ARCH)-elf-ar
USER_ARFLAGS=rcs

ifeq ($(COVERAGE), yes)
	HOST_CFLAGS += \
		-coverage -g

	HOST_LDFLAGS += \
		-coverage
endif
