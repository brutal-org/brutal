.SUFFIXES:
.DELETE_ON_ERROR:
.DEFAULT_GOAL := all
SHELL := /bin/bash

CONFIG?=default
include build/config/$(CONFIG).mk

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
	-Ibin/generated

USER_CFLAGS_INC := \
	-Isources/libs/stdc-ansi \
	-Isources/libs/stdc-math \
	-Isources/libs/stdc-ext \
	-Isources/libs/stdc-posix

CACHEDIR=.cache/
BINDIR=bin/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN)
BINDIR_LOADER=bin/$(CONFIG_ARCH)-loader-$(CONFIG_TOOLCHAIN)
BINDIR_USER=bin/$(CONFIG_ARCH)-brutal-$(CONFIG_TOOLCHAIN)
BINDIR_KERNEL=bin/$(CONFIG_ARCH)-kernel-$(CONFIG_TOOLCHAIN)
BINDIR_HOST=bin/$(CONFIG_HOST_ARCH)-host-$(CONFIG_TOOLCHAIN)

MKCWD=mkdir -p $(@D)

include $(wildcard sources/apps/*/build.mk)
include $(wildcard sources/srvs/*/build.mk)
include $(wildcard sources/utils/*/build.mk)

include build/toolchain/archs/$(CONFIG_ARCH).mk
include build/toolchain/$(CONFIG_TOOLCHAIN)/.build.mk
include sources/build/kernel/build.mk
include sources/build/host/build.mk
include sources/protos/.build.mk
include sources/build/loader/build.mk
include sources/build/target/build.mk

include build/run/$(CONFIG_BOOTLOADER).mk

.PHONY: all
all: $(ALL)

.PHONY: clean
clean:
	rm -rf $(BINDIR)

.PHONY: nuke
nuke:
	rm -rf bin/

-include $(DEPENDENCIES)
