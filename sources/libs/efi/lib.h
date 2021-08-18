#pragma once

#include <efi/tables/st.h>

void efi_init(EFIHandle handle, EFISystemTable *st);

EFIHandle efi_handle(void);

EFISystemTable *efi_st(void);

