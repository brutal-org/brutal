CC = clang
LD = ld.lld

KERNEL_CFLAGS := \
	-D__brutal__ -I../../libs -I../.. -ffreestanding -MMD -mno-red-zone -std=gnu2x \
	-target x86_64-unknown-gnu -Wall -Werror

KERNEL_LDFLAGS := -e kmain -T ./link.ld

SRCS := test.c

default: all

all: kernel.elf

%.o: %.c
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

kernel.elf: test.o
	$(LD) $(KERNEL_LDFLAGS) $< -o $@

-include $(SRCS:.c=.d)

.PHONY: clean all default

clean:
	rm -rf *.d *.o *.elf
