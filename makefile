.SUFFIXES:
.DELETE_ON_ERROR:
.DEFAULT_GOAL:=all
SHELL:=/bin/bash
MKCWD=mkdir -p $(@D)

ARCH?=x86_64
HOST_ARCH?=$(shell uname -m)
BOARD?=pc
BOOTLOADER?=loader
CONFIG?=devel
TOOLCHAIN?=llvm

include sources/build/configs/$(CONFIG).mk
include sources/build/boards/$(ARCH)-$(BOARD)/build.mk


export LC_ALL=C

BASE_CFLAGS += \
	-MD \
	-std=gnu2x \
	-Wall \
	-Wextra \
	-Werror \
	-Wundef \
	-Wshadow \
	-Wvla \
	-Isources/libs/ \
	-Isources/libs/hw \
	-Isources/apps \
	-Isources/srvs \
	-Isources/utils \
	-Isources/ \
	-Ibin/generated \
	-Ibin/generated/headers

USER_CFLAGS_INC := \
	-Isources/libs/stdc-ansi \
	-Isources/libs/stdc-math \
	-Isources/libs/stdc-ext \
	-Isources/libs/stdc-posix

CACHEDIR=.cache/
GENDIR=bin/generated
BINDIR_LOADER=bin/$(CONFIG)/$(ARCH)-loader-$(TOOLCHAIN)
BINDIR_USER=bin/$(CONFIG)/$(ARCH)-brutal-$(TOOLCHAIN)
BINDIR_KERNEL=bin/$(CONFIG)/$(ARCH)-kernel-$(TOOLCHAIN)
BINDIR_HOST=bin/$(CONFIG)/$(HOST_ARCH)-host-$(TOOLCHAIN)

include $(wildcard sources/apps/*/build.mk)
include $(wildcard sources/srvs/*/build.mk)
include $(wildcard sources/utils/*/build.mk)

include build/toolchain/archs/$(ARCH)/arch.mk
-include build/toolchain/archs/$(HOST_ARCH)/host.mk
include build/toolchain/$(TOOLCHAIN)/build.mk

include sources/libs/.build.mk
include sources/build/kernel/build.mk
include sources/build/host/build.mk
include sources/protos/.build.mk
include sources/build/loader/$(BOOTLOADER).mk
include sources/build/target/build.mk

include build/run/$(BOOTLOADER).mk

.PHONY: all
all: $(ALL)

.PHONY: clean
clean:
	rm -rf $(BINDIR_LOADER)
	rm -rf $(BINDIR_USER)
	rm -rf $(BINDIR_KERNEL)
	rm -rf $(BINDIR_HOST)

.PHONY: nuke
nuke:
	rm -rf bin/

-include $(DEPENDENCIES)
