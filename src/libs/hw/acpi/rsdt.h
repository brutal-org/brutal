#pragma once

#include <acpi/base.h>
#include <acpi/stdh.h>

typedef struct PACKED _AcpiRsdt
{
    AcpiSdth stdh;
    uint32_t children[];
} AcpiRsdt;

Iter acpi_rsdt_iterate(Acpi *acpi, IterFn *fn, void *ctx);

Iter acpi_rsdt_lookup(Acpi *acpi, Str sig, IterFn *fn, void *ctx);

AcpiSdth *acpi_rsdt_lookup_first(Acpi *acpi, Str sig);
