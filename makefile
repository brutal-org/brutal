.SUFFIXES:
.DELETE_ON_ERROR:
.DEFAULT_GOAL := all
SHELL := /bin/bash

CONFIG?=default

export LC_ALL=C

CFLAGS_STD ?= \
	-std=gnu2x

CFLAGS_OPT ?= \
	-g -O3

CFLAGS_WARN ?= \
	-Wall \
	-Wextra \
	-Werror \
	-Wundef \
	-Wshadow \
	-Wvla

INC_LIBS= \
	-Isources/libs/stdc-ansi \
	-Isources/libs/stdc-math \
	-Isources/libs/stdc-ext \
	-Isources/libs/stdc-posix

HOST_CFLAGS_SAN ?= \
	-fsanitize=address \
	-fsanitize=undefined

HOST_CFLAGS_INC ?= \
	-Isources/libs/ \
	-Isources/libs/hw \
	-Isources/apps \
	-Isources/srvs \
	-Isources/utils \
	-Isources/ \
	-Ithirdparty

USER_CFLAGS_INC ?= \
	$(INC_LIBS) \
	-Isources/libs/ \
	-Isources/libs/hw \
	-Isources/apps \
	-Isources/srvs \
	-Isources/utils \
	-Isources/ \
	-Ibin/generated \
	-Ithirdparty

CACHEDIR=.cache/
BINDIR=bin/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN)
BINDIR_USER=bin/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN)/user
BINDIR_HOST=bin/$(CONFIG_HOST_ARCH)-$(CONFIG_TOOLCHAIN)/host

MKCWD=mkdir -p $(@D)

include $(wildcard sources/apps/*/build.mk)
include $(wildcard sources/pkgs/*/build.mk)
include $(wildcard sources/utils/*/build.mk)

include build/config/$(CONFIG).mk
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
