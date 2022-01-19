
# x86 use the loader

run: $(LOADER) $(PKGS) $(KERNEL) $(CACHEDIR)/OVMF.fd
	$(MKCWD)

	mkdir -p $(BINDIR_LOADER)/image
	cp -R sysroot/* $(BINDIR_LOADER)/image
	cp $(PKGS) $(BINDIR_LOADER)/image/pkgs

	mkdir -p $(BINDIR_LOADER)/image/EFI/BOOT/
	cp $(KERNEL) $(BINDIR_LOADER)/image/boot/kernel.elf
	cp $(LOADER) $(BINDIR_LOADER)/image/EFI/BOOT/BOOTX64.EFI

	qemu-system-x86_64 \
		$(QEMU_ARGS) \
		-serial mon:stdio \
		-no-reboot \
		-no-shutdown \
		-no-reboot -no-shutdown\
		-bios $(CACHEDIR)/OVMF.fd \
		-drive file=fat:rw:$(BINDIR_LOADER)/image,media=disk,format=raw

