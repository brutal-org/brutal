#include <efi/lib.h>
#include <efi/tty.h>

void efi_tty_reset(void)
{
    efi_st()->console_in->reset(efi_st()->console_in, false);
}

void efi_tty_set_attribute(uint64_t attribute)
{
    efi_st()->console_out->set_attribute(efi_st()->console_out, attribute);
}

void efi_tty_clear(void)
{
    efi_st()->console_out->clear_screen(efi_st()->console_out);
}

EFIInputKey efi_tty_read_key(void)
{
    EFIInputKey key = {};
    uint64_t key_event = 0;

    efi_st()->boot_services->wait_for_event(1, &efi_st()->console_in->wait_for_key, &key_event);
    efi_st()->console_in->read_key(efi_st()->console_in, &key);

    return key;
}
