#pragma once

#include <efi/protos/stip.h>
#include <efi/protos/stop.h>
#include <efi/srvs/bs.h>
#include <efi/srvs/rs.h>
#include <efi/tables/ct.h>

typedef struct
{
    EFITableHeader hdr;
    uint16_t *firmware_vendor;
    uint32_t firmware_revision;
    EFIHandle console_in_handle;
    EFISimpleTextInput *console_in;
    EFIHandle console_out_handle;
    EFISimpleTextOutput *console_out;
    EFIHandle std_error_handle;
    EFISimpleTextOutput *std_error;
    EFIRuntimeServices *runtime_services;
    EFIBootServices *boot_services;
    uint64_t num_table_entries;
    EFIConfigurationTable *config_table;
} EFISystemTable;
