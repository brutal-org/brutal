
# x86 use the loader

IMAGE_ROOT = $(BINDIR_LOADER)/image

run: $(LOADER) $(PKGS) $(KERNEL) $(CACHEDIR)/OVMF.fd
	$(MKCWD)

	mkdir -p $(IMAGE_ROOT)
	cp -R sysroot/* $(IMAGE_ROOT)
	cp $(PKGS) $(IMAGE_ROOT)/pkgs

	mkdir -p $(IMAGE_ROOT)/EFI/BOOT/
	cp $(KERNEL) $(IMAGE_ROOT)/boot/kernel.elf
	cp $(LOADER) $(IMAGE_ROOT)/EFI/BOOT/BOOTX64.EFI

	qemu-system-x86_64 \
		$(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-no-reboot -no-shutdown\
		-bios $(CACHEDIR)/OVMF.fd \
		-drive file=fat:rw:$(IMAGE_ROOT),media=disk,format=raw

