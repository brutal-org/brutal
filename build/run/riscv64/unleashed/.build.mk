
run: $(KERNEL)
	qemu-system-$(CONFIG_ARCH) \
		 $(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-kernel $(KERNEL)
