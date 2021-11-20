.SUFFIXES:

.DEFAULT_GOAL := all
SHELL := /bin/bash

export LC_ALL=C

CFLAGS_STD ?= \
	-std=gnu2x

CFLAGS_OPT ?= \
	-g -O3

CFLAGS_WARN ?= \
	-Wall \
	-Wextra \
	-Werror

INC_LIBS= \
	-Isources/libs/stdc/ansi \
	-Isources/libs/stdc/math \
	-Isources/libs/stdc/bsd \
	-Isources/libs/stdc/gnu \
	-Isources/libs/stdc/posix

INC_LIBBRUTAL= \
	-Isources/libs/

HOST_CFLAGS_INC ?= \
	$(INC_LIBBRUTAL) \
	-Isources/libs/hw \
	-Isources/pkgs \
	-Isources/ \
	-Ithirdparty

USER_CFLAGS_INC ?= \
	$(INC_LIBS) \
	$(INC_LIBBRUTAL) \
	-Isources/libs/hw \
	-Isources/pkgs \
	-Isources/ \
	-Ibin/generated \
	-Ithirdparty

BINDIR=bin/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN)
BINDIR_USER=bin/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN)/user
BINDIR_HOST=bin/$(CONFIG_HOST_ARCH)-$(CONFIG_TOOLCHAIN)/host

MKCWD=mkdir -p $(@D)

include build/config/default.mk
include build/toolchain/archs/$(CONFIG_ARCH).mk
include build/toolchain/$(CONFIG_TOOLCHAIN)/.build.mk

-include sources/pkgs/*/.build.mk
include sources/kernel/.build.mk
include sources/pkgs/.host.mk
include sources/protos/.build.mk
include sources/pkgs/.user.mk
include sources/loader/.build.mk

.PHONY: all
all: $(ALL)

run-riscv: $(KERNEL)
	qemu-system-$(CONFIG_ARCH) \
		 $(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-kernel $(KERNEL)

run: $(ISO)
	qemu-system-$(CONFIG_ARCH) \
		$(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-cdrom $(ISO)


.PHONY: clean
clean:
	rm -rf $(BINDIR)

.PHONY: clean
nuke:
	rm -rf bin/

-include $(DEPENDENCIES)
