
# x86 use the loader

DISK=$(BINDIR_LOADER)/disk.hdd

image: $(LOADER) $(PKGS) $(KERNEL) $(BINDIR_LOADER)/tools/OVMF.fd 
	mkdir -p $(BINDIR_LOADER)/image
	cp -R sysroot/* $(BINDIR_LOADER)/image
	cp $(PKGS) $(BINDIR_LOADER)/image/pkgs

	mkdir -p $(BINDIR_LOADER)/image/EFI/BOOT/
	cp $(KERNEL) $(BINDIR_LOADER)/image/boot/kernel.elf
	cp $(LOADER) $(BINDIR_LOADER)/image/EFI/BOOT/BOOTX64.EFI

$(DISK): image
	./build/utils/make-maindisk.sh

run: $(DISK)
	$(MKCWD)
	
	qemu-system-x86_64 \
		$(QEMU_ARGS) \
		-serial stdio \
		-no-reboot \
		-no-shutdown \
		-no-reboot -no-shutdown\
		-bios $(BINDIR_LOADER)/tools/OVMF.fd \
		-d guest_errors \
		-drive file=$(DISK),format=raw,if=none,media=disk,id=boot_disk \
		-device ahci,id=achi0 \
		-device ide-hd,bus=achi0.0,drive=boot_disk,bootindex=1 \
		-M q35
