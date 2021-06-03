.SUFFIXES:
.DEFAULT_GOAL := all

CC=gcc

CFLAGS= \
	-std=gnu2x \
	-Wall \
	-Wextra \
	-Werror \
	-O2 \
	-Ideps \
	-Isources/ \
	-Isources/lib

KCFLAGS= \
	$(CFLAGS) \
	-ffreestanding \
	-fno-stack-protector \
	-fno-pic \
	-fpie \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone

KLDFLAGS= \
	-fno-pic \
	-fpie \
	-Wl,-static,-pie,--no-dynamic-linker,-ztext \
	-static-pie \
	-nostdlib \
	-Tsources/arch/link.ld \
	-z max-page-size=0x1000

AR=ar
ARFLAGS=rcsv

MKCWD=mkdir -p $(@D)

include sources/arch/.build.mk
include sources/kernel/.build.mk
include sources/lib/.build.mk
include sources/sysroot/.build.mk

all: $(SYSROOT_ISO)

run: $(SYSROOT_ISO)
	qemu-system-x86_64 -M q35 -m 2G -serial mon:stdio -cdrom $(SYSROOT_ISO)

clean:
	rm -rf build/
