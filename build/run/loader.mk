
ifneq ("$(QEMU_KVM)","no")
	ifneq ("$(wildcard /dev/kvm)","")
		QEMU_ARGS += -enable-kvm
	endif
endif

# x86 use the loader

IMAGE_ROOT = $(BINDIR_LOADER)/image

$(CACHEDIR)/OVMF.fd:
	$(MKCWD)
	wget https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd
	mv DEBUGX64_OVMF.fd $@

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
		-no-reboot -no-shutdown \
		-bios $(CACHEDIR)/OVMF.fd \
		-drive file=fat:rw:$(IMAGE_ROOT),media=disk,format=raw \
		-M q35 -net nic,model=rtl8139 -net nic,model=e1000

