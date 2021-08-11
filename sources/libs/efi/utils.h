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
void clear_screen();
void set_attribute(u64 attr);
void panic(char *str, ...);

#define TO_UTF16(ptr, buffer)                                     \
    void *ptr = efi_malloc(strlen(buffer) << 1);                  \
    memset(ptr, 0, sizeof(ptr));                                  \
    for (size_t i = 0; i < sizeof(buffer) / sizeof(*buffer); i++) \
    {                                                             \
        *(char16 *)(ptr + (i << 1)) = buffer[i];                  \
    }

#define UNWRAP_WITH_MSG(expr, msg, ...) (           \
    {                                               \
        auto expr_value = (expr);                   \
                                                    \
        if (!expr_value.success)                    \
        {                                           \
            panic(msg, __VA_OPT__(, ) __VA_ARGS__); \
        }                                           \
                                                    \
        expr_value._ok;                             \
    })
