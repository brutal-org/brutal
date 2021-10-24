#pragma once

#include <acpi/base.h>
#include <acpi/stdh.h>

typedef enum
{
    ACPI_MADT_RECORD_LAPIC = 0,
    ACPI_MADT_RECORD_IOAPIC = 1,
    ACPI_MADT_RECORD_ISO = 2,
    ACPI_MADT_RECORD_NMI = 4,
    ACPI_MADT_RECORD_LAPIC_OVERRIDE = 5
} AcpiMadtRecordType;

typedef struct
{
    uint8_t type;
    uint8_t len;
} AcpiMadtRecord;

#define ACPI_MADT_SIG str$("APIC")

typedef struct PACKED
{
    AcpiSdth sdth;

    uint32_t lapic;
    uint32_t flags;

    AcpiMadtRecord records[];
} AcpiMadt;

typedef struct PACKED
{
    AcpiMadtRecord record;

    uint8_t processor_id;
    uint8_t id;
    uint32_t flags;
} AcpiMadtLapicRecord;

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

Iter acpi_madt_iterate(Acpi *acpi, IterFn *fn, void *ctx);

Iter acpi_madt_lookup(Acpi *acpi, AcpiMadtRecordType type, IterFn *fn, void *ctx);
