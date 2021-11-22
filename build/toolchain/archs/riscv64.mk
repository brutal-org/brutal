
ARCH_CFLAGS = \
	-march=rv64imafdc \
	-D__$(CONFIG_BOARD)=1 \
	-mno-relax

USER_AS=$(CONFIG_ARCH)-elf-as
USER_ASFLAGS=-mno-relax -march=rv64imafdc

QEMU_ARGS= \
		--machine sifive_u -d cpu_reset

run-riscv: $(KERNEL)
	qemu-system-$(CONFIG_ARCH) \
		 $(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-kernel $(KERNEL)
