
ARCH_CFLAGS = \
	-march=rv64imafdc

ARCH_LDFLAGS=

CROSS_AS=$(CONFIG_ARCH)-elf-as
CROSS_ASFLAGS=

QEMU_ARGS= \
		--machine sifive_u
