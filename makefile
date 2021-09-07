.SUFFIXES:

.DEFAULT_GOAL := all
export LC_ALL=C

ifndef ECHO
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
      -nrRf $(firstword $(MAKEFILE_LIST)) \
      ECHO="%COUNT_ME%" | grep -c "%COUNT_ME%")

N := x
C = $(words $N)$(eval N := x $N)

ECHO = @echo -e "`printf "\e[1m %3d%%\e[m" $$(expr $C '*' 100 / $T)`"
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
	-Isources/libs/ansi \
	-Isources/libs/posix \
	-Isources/libs/bsd \
	-Isources/libs/gnu \
	-Isources/libs/math

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
include meta/toolchain/$(CONFIG_TOOLCHAIN)/.build.mk

include sources/kernel/.build.mk
include sources/bins/.cross.mk
include sources/bins/.host.mk
include sources/loader/.build.mk
include sysroot/.build.mk
include sources/libs/proto/.build.mk

.PHONY: all
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
		-rtc base=localtime \
		-cdrom $(ISO)

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
