#pragma once

#include <brutal/base.h>

typedef struct PACKED
{
    char signature[8];
    uint8_t checksum;
    char OEM_id[6];
    uint8_t revision;
    uint32_t rsdt;
} AcpiRsdp;



typedef struct
{
    AcpiSdth stdh;
    uint32_t children[];
} AcpiRsdt;



t
