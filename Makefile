.SUFFIXES:
.DEFAULT_GOAL := all

export PATH := $(shell toolchain/use-it.sh):$(PATH)
export LC_ALL=C

STD = \
	-std=gnu2x

OPTIMISATIONS= \
	-O2

WARNINGS= \
	-Wall \
	-Wextra \
	-Werror

INCLUDES=\
	-Ithirdparty \
	-Isources/ \
	-Isources/library

HOST_CC=cc

HOST_CFLAGS= \
	$(STD) \
	$(OPTIMISATIONS) \
	$(WARNINGS) \
	$(INCLUDES)

CROSS_AS=nasm
CROSS_ASFLAGS=-f elf64

CROSS_CC=x86_64-elf-gcc
CROSS_CFLAGS= \
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

CROSS_LD=x86_64-elf-ld
CROSS_KLDFLAGS= \
	-Tsources/arch/link.ld \
	-z max-page-size=0x1000

CROSS_AR=x86_64-elf-ar
CROSS_ARFLAGS=rcsv

MKCWD=mkdir -p $(@D)

include sources/arch/.build.mk
include sources/kernel/.build.mk
include sources/library/.build.mk
include sources/sysroot/.build.mk

all: $(TARGETS)

run: $(SYSROOT_ISO)
	qemu-system-x86_64 \
		-M q35 \
		-m 2G \
		-smp 8 \
		-serial mon:stdio \
		-display sdl \
		-cdrom $(SYSROOT_ISO)

clean:
	rm -rf build/
