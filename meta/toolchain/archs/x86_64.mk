
ARCH_KCFLAGS = \
	-fno-stack-protector \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel 

ARCH_CFLAGS = \
	-fsanitize=undefined 

CROSS_AS=nasm
CROSS_ASFLAGS=-f elf64

ARCH_LDFLAGS = \
	meta/toolchain/gnu/local/lib/gcc/x86_64-elf/11.1.0/libgcc.a

QEMU_ARGS= \
		-M q35 \
		-cpu host \
		-smp 4 \
		-m 256M \
		-enable-kvm \
		-rtc base=localtime 

QEMU_BOOT_METHOD=iso
