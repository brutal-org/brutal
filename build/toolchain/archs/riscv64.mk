
ARCH_CFLAGS = \
	-march=rv64imafdc \
	-D__$(CONFIG_BOARD)=1 \
	-mno-relax \
	-mcmodel=medany

USER_AS=$(CONFIG_ARCH)-elf-as
USER_ASFLAGS=-mno-relax -march=rv64imafdc

QEMU_ARGS= \
		--machine virt -d cpu_reset
