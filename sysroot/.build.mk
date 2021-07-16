LIMINE=thirdparty/limine/limine-install
SYSROOT=$(BUILDDIR_CROSS)/sysroot
ISO=$(BUILDDIR_CROSS)/sysroot.iso

$(LIMINE):
	make -C thirdparty/limine all

$(SYSROOT): sysroot/boot/limine.cfg $(KERNEL) $(SERVERS)
	$(MKCWD)
	rm -rf $(SYSROOT)
	cp -R sysroot                                $(SYSROOT)

	cp $(KERNEL)                                 $(SYSROOT)/boot/kernel.elf
	cp thirdparty/limine/limine.sys              $(SYSROOT)/boot/limine.sys
	cp thirdparty/limine/limine-cd.bin           $(SYSROOT)/boot/limine-cd.bin
	cp thirdparty/limine/limine-eltorito-efi.bin $(SYSROOT)/boot/limine-eltorito-efi.bin

	cp $(SERVERS)                                $(SYSROOT)/servers

$(ISO): $(SYSROOT) $(LIMINE)
	$(MKCWD)

	xorriso \
		-as mkisofs \
		-b boot/limine-cd.bin \
		-no-emul-boot \
		-boot-load-size $(shell nproc --all) \
		-boot-info-table \
		--efi-boot boot/limine-eltorito-efi.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label \
		$(SYSROOT) \
		-o $(ISO)

	$(LIMINE) $(ISO)
