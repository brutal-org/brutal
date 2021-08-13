#pragma once
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/host/mem.h>
#include <brutal/log.h>
#include <efi/base.h>
#include <efi/file.h>
#include <efi/lip.h>
#include <efi/sfsp.h>
#include <efi/st.h>

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

void *to_utf16(char *buffer);

void set_attribute(u64 attribute);

void clear_screen();

void efi_print_impl(Str fmt, struct print_args args);

#define efi_print(fmt, ...) \
    efi_print_impl(str_cast(fmt), PRINT_ARGS(__VA_ARGS__))
