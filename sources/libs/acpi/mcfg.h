#pragma once

#include <acpi/stdh.h>

typedef struct PACKED
{
    uint64_t address;
    uint16_t segment_groupe;
    uint8_t bus_start;
    uint8_t bus_end;
    uint32_t reserved;
} AcpiMcfgRecord;

typedef struct PACKED
{
    AcpiSdth stdh;
    uint64_t reserved;
    AcpiMcfgRecord records[];
} AcpiMcfg;
