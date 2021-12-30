#pragma once

#include <efi/protos/stip.h>

void efi_tty_reset(void);

void efi_tty_set_attribute(uint64_t attribute);

void efi_tty_clear(void);

EFIInputKey efi_tty_read_key(void);
