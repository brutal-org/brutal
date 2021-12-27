export PATH := $(shell build/toolchain/gnu/use.sh):$(PATH)

# --- Host compiler ---------------------------------------------------------- #

HOST_CC=cc

HOST_CFLAGS= \
	-MD \
	-fanalyzer \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(HOST_CFLAGS_INC) \
	-fsanitize=address \
	-fsanitize=undefined \
	`pkg-config  sdl2 --cflags`

HOST_LD=ld
HOST_LDFLAGS= \
	-lm \
	`pkg-config  sdl2 --libs`

HOST_AR=ar
HOST_ARFLAGS=rcs

# default can be changed in archs

USER_AS?=$(CONFIG_ARCH)-elf-as

# --- User-Compiler ---------------------------------------------------------- #
USER_CC=$(CONFIG_ARCH)-elf-gcc

USER_CFLAGS= \
	-MD \
	$(CFLAGS_STD) \
	$(CFLAGS_OPT) \
	$(CFLAGS_WARN) \
	$(ARCH_CFLAGS) \
	$(USER_CFLAGS_INC) \
	-D__brutal__=1 \
	-ffreestanding

USER_KCFLAGS= \
	$(USER_CFLAGS) \
	$(ARCH_KCFLAGS) \
	-fno-stack-protector \
	-D__kernel__=1

USER_UCFLAGS= \
	$(USER_CFLAGS) \
	-nostdlib

USER_LD=$(CONFIG_ARCH)-elf-ld
USER_KLDFLAGS= \
	-Tsources/embed/kernel/$(CONFIG_ARCH)/$(CONFIG_BOARD)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS) \
	build/toolchain/gnu/local/lib/gcc/$(CONFIG_ARCH)-elf/11.1.0/libgcc.a

USER_ULDFLAGS= \
	-Tsources/embed/brutal/$(CONFIG_ARCH)/link.ld \
	-z max-page-size=0x1000 \
	$(ARCH_LDFLAGS) \
	build/toolchain/gnu/local/lib/gcc/$(CONFIG_ARCH)-elf/11.1.0/libgcc.a

USER_OBJCOPY=$(CONFIG_ARCH)-objcopy

USER_AR=$(CONFIG_ARCH)-elf-ar
USER_ARFLAGS=rcs

ifeq ($(COVERAGE), yes)
	HOST_CFLAGS += \
		-coverage -g

	HOST_LDFLAGS += \
		-coverage
endif
