LIMINE=thirdparty/limine/limine-install
SYSROOT=build/sysroot
SYSROOT_ISO=build/sysroot.iso

$(LIMINE):
	make -C thirdparty/limine all

$(SYSROOT): sources/sysroot/limine.cfg $(KERNEL_BIN)
	$(MKCWD)
	mkdir -p $(SYSROOT)

	cp sources/sysroot/limine.cfg           $(SYSROOT)/limine.cfg
	cp $(KERNEL_BIN)                        $(SYSROOT)/kernel.elf
	cp thirdparty/limine/limine.sys               $(SYSROOT)/limine.sys
	cp thirdparty/limine/limine-cd.bin            $(SYSROOT)/limine-cd.bin
	cp thirdparty/limine/limine-eltorito-efi.bin  $(SYSROOT)/limine-eltorito-efi.bin

$(SYSROOT_ISO): $(SYSROOT) $(LIMINE)
	$(MKCWD)

	xorriso \
		-as mkisofs \
		-b limine-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label \
		$(SYSROOT) \
		-o $(SYSROOT_ISO)

	$(LIMINE) $(SYSROOT_ISO)
