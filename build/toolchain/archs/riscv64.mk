
ARCH_CFLAGS = \
	-march=rv64imafdc \
	-D__$(CONFIG_BOARD)=1 
USER_AS=$(CONFIG_ARCH)-elf-as
USER_ASFLAGS=

QEMU_ARGS= \
		--machine sifive_u
