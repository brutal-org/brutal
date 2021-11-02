#pragma once

#include <acpi/stdh.h>

#define ACPI_MCFG_SIG str$("MCFG")

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

static inline int acpi_mcfg_record_count(AcpiMcfg *mcfg)
{
    return (mcfg->stdh.len - sizeof(AcpiMcfg)) / sizeof(AcpiMcfgRecord);
}
