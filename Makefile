.SUFFIXES:
.DEFAULT_GOAL := all

export PATH := $(shell toolchain/use-it.sh):$(PATH)
export LC_ALL=C

STD = \
	-std=gnu2x

OPTIMISATIONS= \
	-g

WARNINGS= \
	-Wall \
	-Wextra \
	-Werror

INCLUDES=\
	-Ithirdparty \
	-Isources/

# --- Hosted build ----------------------------------------------------------- #

HOST_BUILDDIR=build/host

HOST_CC=cc

HOST_CFLAGS= \
	$(STD) \
	$(OPTIMISATIONS) \
	$(WARNINGS) \
	$(INCLUDES)

HOST_LD=ld
HOST_LDFLAGS=

# --- Cress build ------------------------------------------------------------ #

CROSS_BUILDDIR=build/cross

CROSS_AS=nasm
CROSS_ASFLAGS=-f elf64

CROSS_CC=clang
CROSS_CFLAGS= \
	-target x86_64-none-elf \
	-MD \
	$(STD) \
	$(OPTIMISATIONS) \
	$(WARNINGS) \
	$(INCLUDES) \
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

CROSS_LD=clang
CROSS_KLDFLAGS= \
	-Tsources/arch/x86_64/link.ld \
	-nostdlib \
	-z max-page-size=0x1000

CROSS_AR=llvm-ar
CROSS_ARFLAGS=rcsv

MKCWD=mkdir -p $(@D)

include sources/arch/.build.mk
include sources/kernel/.build.mk
include sources/library/.build.mk
include sources/sysroot/.build.mk
include sources/test/.build.mk

all: $(TARGETS)

run: $(SYSROOT_ISO)
	qemu-system-x86_64 \
		-M q35 \
		-m 2G \
		-smp 8 \
		-enable-kvm \
		-cpu host \
		-serial mon:stdio \
		-display sdl \
		-cdrom $(SYSROOT_ISO)

clean:
	rm -rf build/

-include $(DEPENDENCIES)