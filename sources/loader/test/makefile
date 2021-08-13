CC = clang
LD = lld

KERNEL_CFLAGS := \
	-ffreestanding -MMD -mno-red-zone -std=c11 \
	-target x86_64-unknown-gnu -Wall -Werror -pedantic

KERNEL_LDFLAGS := \
	-flavor ld -e main

SRCS := test.c

default: all

all: test.elf

%.o: %.c
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

test.elf: test.o
	$(LD) $(KERNEL_LDFLAGS) $< -o $@
	cp config.cfg $@ ../../../build/loader/image/EFI

-include $(SRCS:.c=.d)

.PHONY: clean all default

clean:
	rm -rf *.d *.o *.elf
