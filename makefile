.SUFFIXES:
.DEFAULT_GOAL := all
export LC_ALL=C

CFLAGS_STD ?= \
	-std=gnu2x

CFLAGS_OPT ?= \
	-O3

CFLAGS_WARN ?= \
	-Wall \
	-Wextra \
	-Werror

INC_LIBS= \
	-Isources/libs/ansi \
	-Isources/libs/posix \
	-Isources/libs/bsd \
	-Isources/libs/gnu

INC_LIBBRUTAL= \
	-Isources/libs/

HOST_CFLAGS_INC ?= \
	$(INC_LIBBRUTAL) \
	-Isources/ \
	-Ithirdparty

CROSS_CFLAGS_INC ?= \
	$(INC_LIBS) \
	$(INC_LIBBRUTAL) \
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
include sources/libs/.build.mk
include sources/crts/.build.mk
include sources/bins/.build.mk
include sources/test/.build.mk
include sysroot/.build.mk

all: $(ISO)

run: $(ISO)
	qemu-system-x86_64 \
		-M q35 \
		-cpu host \
		-smp 4 \
		-m 256M \
		-enable-kvm \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-cdrom $(ISO) \
		-d guest_errors

bochs:
	bochs

clean:
	rm -rf build/

-include $(DEPENDENCIES)
