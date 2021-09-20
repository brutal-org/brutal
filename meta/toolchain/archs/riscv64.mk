
ARCH_CFLAGS = \
	-march=rv64imafdc

ARCH_LDFLAGS= \
	meta/toolchain/gnu/local/lib/gcc/riscv64-elf/11.1.0/rv64imac/lp64/libgcc.a 

CROSS_AS=$(CONFIG_ARCH)-elf-as
CROSS_ASFLAGS=

QEMU_ARGS= \
		--machine sifive_u
