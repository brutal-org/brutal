#include <brutal/debug.h>
#include <efi/lib.h>
#include <efi/tty.h>
#include <loader/loader.h>

EfiStatus efi_main(EFIHandle handle, EFISystemTable *st)
{
    efi_init(handle, st);

    st->boot_services->set_watchdog_timer(0, 0, 0, nullptr);

    efi_tty_reset();

    loader_entry();

    panic$("loader_entry shouldn't return!");
}

void loader_boot_deinit(void)
{
    efi_deinit();
}
