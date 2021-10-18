.SUFFIXES:

.DEFAULT_GOAL := all
SHELL := /bin/bash

export LC_ALL=C

ifndef ECHO
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
      -nrRf $(firstword $(MAKEFILE_LIST)) \
      ECHO="%COUNT_ME%" | grep -c "%COUNT_ME%")

N := x
C = $(words $N)$(eval N := x $N)

ECHO = @echo -e "[$C/$T]"
endif

CFLAGS_STD ?= \
	-std=gnu2x

CFLAGS_OPT ?= \
	-O3

CFLAGS_WARN ?= \
	-Wall \
	-Wextra \
	-Werror

INC_LIBS= \
	-Isources/libs/stdc/ansi \
	-Isources/libs/stdc/posix \
	-Isources/libs/stdc/bsd \
	-Isources/libs/stdc/gnu \
	-Isources/libs/stdc/math

INC_LIBBRUTAL= \
	-Isources/libs/

HOST_CFLAGS_INC ?= \
	$(INC_LIBBRUTAL) \
	-Isources/bins \
	-Isources/ \
	-Ithirdparty

CROSS_CFLAGS_INC ?= \
	$(INC_LIBS) \
	$(INC_LIBBRUTAL) \
	-Isources/bins \
	-Isources/ \
	-Ithirdparty

BUILDDIR=build/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN)
MKCWD=mkdir -p $(@D)

include meta/config/default.mk
include meta/toolchain/archs/$(CONFIG_ARCH).mk
include meta/toolchain/$(CONFIG_TOOLCHAIN)/.build.mk

include sources/kernel/.build.mk
include sources/bins/.cross.mk
include sources/bins/.host.mk
include sources/loader/.build.mk
include sysroot/.build.mk
include sources/libs/proto/.build.mk

.PHONY: all
all: $(ISO)

ifeq ($(QEMU_BOOT_METHOD), kernel)

run: $(ISO)
	qemu-system-$(CONFIG_ARCH) \
		 $(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-kernel $(KERNEL)
		-hda $(ISO) 
		
else ifeq ($(QEMU_BOOT_METHOD), iso)

run: $(ISO)
	qemu-system-$(CONFIG_ARCH) \
		$(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-cdrom $(ISO)

endif

.PHONY: bochs
bochs: $(ISO)
	bochs

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: clean
clean-all:
	rm -rf build/

-include $(DEPENDENCIES)
