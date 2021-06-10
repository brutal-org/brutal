.SUFFIXES:
.DEFAULT_GOAL := all
export LC_ALL=C

CFLAGS_STD ?= \
	-std=gnu2x

CFLAGS_OPT ?= \
	-g

CFLAGS_WARN ?= \
	-Wall \
	-Wextra \
	-Werror

CFLAGS_INC ?= \
	-Isources/libc/ansi \
	-Isources/libc/posix \
	-Isources/libc/ \
	-Isources/ \
	-Ithirdparty

BUILDDIR=build/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN)
BUILDDIR_HOST=$(BUILDDIR)/host
BUILDDIR_CROSS=$(BUILDDIR)/cross

MKCWD=mkdir -p $(@D)

include meta/config/default.mk
include meta/toolchain/$(CONFIG_TOOLCHAIN)/.build.mk

include sources/arch/.build.mk
include sources/kernel/.build.mk
include sources/libc/.build.mk
include sources/sysroot/.build.mk
include sources/test/.build.mk

all: $(TARGETS)

run: $(SYSROOT_ISO)
	qemu-system-x86_64 \
		-M q35 \
		-m 2G \
		-smp 12 \
		-enable-kvm \
		-cpu host \
		-serial mon:stdio \
		-display sdl \
		-cdrom $(SYSROOT_ISO)

bochs: 
	bochs
	
clean:
	rm -rf build/

-include $(DEPENDENCIES)
