
ARCH_CFLAGS = \
	-march=rv64imafdc \
	-D__$(CONFIG_BOARD)=1 
CROSS_AS=$(CONFIG_ARCH)-elf-as
CROSS_ASFLAGS=

QEMU_ARGS= \
		--machine sifive_u

QEMU_BOOT_METHOD=kernel
