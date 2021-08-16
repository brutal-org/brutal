#pragma once

#include <acpi/stdh.h>

typedef enum
{
    ACPI_MADT_RECORD_LAPIC = 0,
    ACPI_MADT_RECORD_IOAPIC = 1,
    ACPI_MADT_RECORD_MADT_ISO = 2,
    ACPI_MADT_RECORD_NMI = 4,
    ACPI_MADT_RECORD_LAPIC_OVERRIDE = 5
} AcpiMadtRecordType;

typedef struct
{
    uint8_t type;
    uint8_t len;
} AcpiMadtRecord;

typedef struct PACKED
{
    AcpiSdth sdth;

    uint32_t lapic;
    uint32_t flags;

    AcpiMadtRecord records[];
} AcpiMadt;

typedef struct PACKED
{
    uint8_t type;
    uint8_t len;
} AcpiMadtRecord;

typedef struct PACKED
{
    AcpiMadtRecord record;

    uint8_t processor_id;
    uint8_t id;
    uint32_t flags;
};

typedef struct PACKED
{
    AcpiMadtRecord record;

    uint8_t id;
    uint8_t reserved;
    uint32_t address;
    uint32_t interrupt_base;
} AcpiMadtIoapicRecord;

typedef struct PACKED
{
    AcpiMadtRecord record;

    uint8_t bus;
    uint8_t irq;
    uint32_t gsi;
    uint16_t flags;
} AcpiMadtIsoRecord;
