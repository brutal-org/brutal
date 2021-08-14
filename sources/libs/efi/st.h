#pragma once
#include <efi/base.h>
#include <efi/bs.h>
#include <efi/console.h>
#include <efi/ct.h>
#include <efi/rs.h>

typedef struct
{
    EFITableHeader hdr;
    char16 *firmware_vendor;
    u32 firmware_revision;
    EFIHandle console_in_handle;
    EFISimpleTextInput *console_in;
    EFIHandle console_out_handle;
    EFISimpleTextOutput *console_out;
    EFIHandle std_error_handle;
    EFISimpleTextOutput *std_error;
    EFIRuntimeServices *runtime_services;
    EFIBootServices *boot_services;
    u64 num_table_entries;
    EFIConfigurationTable *config_table;
} EFISystemTable;
