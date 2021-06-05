#pragma once

#include <library/base/macros.h>
#include <library/base/std.h>

struct PACKED acpi_rsdp
{
    char signature[8];
    uint8_t checksum;
    char OEM_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
};

struct PACKED acpi_sdth
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

struct PACKED acpi_rsdt
{
    struct acpi_sdth base;
    uint32_t children[];
};

enum
{
    ACPI_MADT_RECORD_LAPIC = 0,
    ACPI_MADT_RECORD_IOAPIC = 1,
};

struct PACKED acpi_madt_record
{
    uint8_t type;
    uint8_t lenght;
};

struct PACKED acpi_madt_lapic_record
{
    struct acpi_madt_record base;

    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
};

struct PACKED acpi_madt_ioapic_record
{
    struct acpi_madt_record base;

    uint8_t id;
    uint8_t reserved;
    uint32_t address;
    uint32_t interrupt_base;
};

struct PACKED acpi_madt
{
    struct acpi_sdth base;

    uint32_t local_apic;
    uint32_t flags;

    struct acpi_madt_record records[];
};

uint32_t acpi_find_lapic(uintptr_t acpi_rsdp);

uint32_t acpi_find_ioapic(uintptr_t acpi_rsdp);
