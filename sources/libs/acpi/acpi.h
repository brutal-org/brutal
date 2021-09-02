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
    uintptr_t base;
    AcpiRsdp *rsdp;
    struct acpi_rsdt *rsdt;
} Acpi;

void acpi_init(Acpi *acpi, uintptr_t base, uintptr_t rsdp);

uintptr_t acpi_phys_to_virt(Acpi *acpi, uintptr_t addr);
