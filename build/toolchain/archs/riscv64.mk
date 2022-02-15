
ARCH_CFLAGS = \
	-march=rv64imafdc \
	-D__$(CONFIG_BOARD)=1 \

ARCH_KCFLAGS = \
	-mno-relax \
	-mcmodel=medany -fPIC -fpie

ARCH_KLDFLAGS = \
	--pie

USER_ASFLAGS=-fpie -fPIC -mno-relax -march=rv64imafdc -target $(CONFIG_ARCH)-none-elf -c

QEMU_ARGS = \
	--machine virt -d cpu_reset  -m 128M