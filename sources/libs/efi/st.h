#pragma once
#include <efi/base.h>
#include <efi/bs.h>
#include <efi/console.h>
#include <efi/rs.h>

typedef struct
{
    char a[36];
    EFIHandle console_in_handle;
    EFISimpleTextInput *console_in;
    EFIHandle console_out_handle;
    EFISimpleTextOutput *console_out;
    char b[16];
    EFIRuntimeServices *runtime_services;
    EFIBootServices *boot_services;
} EFISystemTable;
