#include <efi/lib.h>
#include <efi/protos.h>
#include <brutal/debug.h>
#include <efi/tty.h>
#include <embed/boot.h>


EfiStatus efi_main(EFIHandle handle, EFISystemTable *st)
{
    efi_init(handle, st);

    st->boot_services->set_watchdog_timer(0, 0, 0, nullptr);

    efi_tty_reset();

	loader_boot_impl();

    panic$("loader_boot_impl shouldn't return!");
}

void embed_boot_deinit(void)
{
	efi_deinit();
}
