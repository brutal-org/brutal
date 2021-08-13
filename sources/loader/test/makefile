CC = clang
LD = ld.lld

KERNEL_CFLAGS := \
	-D__brutal__ -I../../libs -ffreestanding -MMD -mno-red-zone -std=gnu2x \
	-target x86_64-unknown-gnu -Wall -Werror

KERNEL_LDFLAGS := -e kmain

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
