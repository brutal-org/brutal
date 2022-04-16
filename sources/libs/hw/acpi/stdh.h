#pragma once

#include <brutal-base>
#include <brutal-text>

typedef struct PACKED
{
    char signature[4];
    uint32_t len;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} AcpiSdth;

static inline Str acpi_sdth_sig(AcpiSdth *sdth)
{
    return str_n$(4, sdth->signature);
}
