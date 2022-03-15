LOADER:=$(CACHEDIR)/limine/BOOTX64.EFI

$(LOADER): $(LOADER_OBJS)
	@$(MKCWD)
	wget https://github.com/limine-bootloader/limine/raw/latest-binary/BOOTX64.EFI
	mv BOOTX64.EFI $@
