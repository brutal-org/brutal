#pragma once

#include <efi/tables/st.h>

void efi_init(EFIHandle handle, EFISystemTable *st);

void efi_deinit(void);

EFIHandle efi_handle(void);

EFISystemTable *efi_st(void);

/* you should free EFIMemoryDescriptor if you don't need it */
EFIMemoryDescriptor *efi_mmap_snapshot(size_t *size, size_t *desc_size);
