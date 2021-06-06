#include <library/base/cast.h>
#include <library/text/str.h>

#include "arch/x86_64/acpi.h"

struct acpi_sdth *acpi_rsdt_child(struct acpi_rsdt *rsdt, str_t signature)
{
    size_t child_count = (rsdt->base.length - sizeof(rsdt->base)) / 4;

    for (size_t i = 0; i < child_count; i++)
    {
        // FIXME: sketchy cast
        auto sdth = cast_from_uint32(struct acpi_sdth *, rsdt->children[i]);

        if (str_eq(signature, make_str_n(sdth->signature, 4)))
        {
            return sdth;
        }
    }

    return nullptr;
}

struct acpi_madt_record *acpi_madt_record(
    struct acpi_madt *madt,
    enum acpi_madt_record_type type)
{
    struct acpi_madt_record *current = madt->records;

    while ((uintptr_t)current < (uintptr_t)&madt->base + madt->base.length)
    {
        if (current->type == type)
        {
            return current;
        }

        current = (struct acpi_madt_record *)(((uintptr_t)current) + current->lenght);
    }

    return nullptr;
}

struct acpi_madt *acpi_find_madt(uintptr_t rsdp_address)
{
    auto rsdp = cast_from_uint32(struct acpi_rsdp *, rsdp_address);
    auto rsdt = cast_from_uint32(struct acpi_rsdt *, rsdp->rsdt_address);

    return (struct acpi_madt *)acpi_rsdt_child(rsdt, make_str("APIC"));
}

uint32_t acpi_find_lapic(uintptr_t rsdp_address)
{
    auto madt = acpi_find_madt(rsdp_address);

    return madt->local_apic;
}

uint32_t acpi_find_ioapic(uintptr_t rsdp_address)
{
    auto madt = acpi_find_madt(rsdp_address);
    auto ioapic = (struct acpi_madt_ioapic_record *)acpi_madt_record(madt, ACPI_MADT_RECORD_IOAPIC);

    return ioapic->address;
}
