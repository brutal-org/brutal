
ARCH_KCFLAGS = \
	-fno-stack-protector \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel

USER_AS=nasm
USER_ASFLAGS=-f elf64

QEMU_ARGS= \
		-M q35 \
		-smp 4 \
		-m 256M \
		-rtc base=localtime
