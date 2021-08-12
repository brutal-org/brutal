#pragma once
#include <brutal/base.h>
#include <efi/base.h>
#include <efi/file.h>
#include <efi/lip.h>
#include <efi/sfsp.h>
#include <efi/st.h>
#include <string.h>

typedef struct
{
    EFIFileInfo info;
    EFIFileProtocol *efi_file;
    void *buffer;
    EFIStatus status;
} File;

void init_lib(EFISystemTable *st, EFIHandle image_handle);
void read_file(File *file, void *buffer);
File open_file(u16 *path);
void close_file(File *file);
EFIFileInfo get_file_info(File *file);
void efi_printf(char *fmt, ...);
void *efi_malloc(u64 size);
void efi_free(void *ptr);
void *to_utf16(void* ptr, char *buffer);
