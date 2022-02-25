
run: $(KERNEL)
	qemu-system-$(ARCH) \
		 $(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-kernel $(KERNEL)
