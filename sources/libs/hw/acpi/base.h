#pragma once

#include <brutal-base>
#ifndef __kernel__
#    include <bal/hw.h>
#endif

#define ACPI_WINDOW_SIZE

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
#ifdef __kernel__
    uintptr_t base;
#else
    BalMem mem;
#endif

    AcpiRsdp *rsdp;
    struct _AcpiRsdt *rsdt;
} Acpi;

#ifdef __kernel__
void acpi_init(Acpi *acpi, uintptr_t base, uintptr_t rsdp);
#else
void acpi_init(Acpi *acpi, uintptr_t rsdp);
#endif

void acpi_deinit(Acpi *acpi);

uintptr_t acpi_phys_to_virt(Acpi *acpi, uintptr_t addr);
