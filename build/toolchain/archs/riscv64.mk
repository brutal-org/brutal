
ARCH_CFLAGS = \
	-march=rv64imafdc \
	-D__$(CONFIG_BOARD)=1 \
	-mno-relax \
	-mcmodel=medany

USER_ASFLAGS=-mno-relax -march=rv64imafdc -target $(CONFIG_ARCH)-none-elf -c

QEMU_ARGS = \
	--machine virt -d cpu_reset
