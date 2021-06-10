#include <brutal/base/cast.h>
#include <brutal/log.h>
#include <brutal/text/str.h>
#include "arch/x86_64/acpi.h"
#include "arch/x86_64/memory/mmap.h"

struct acpi_sdth *acpi_rsdt_child(struct acpi_rsdt *rsdt, str_t signature)
{
    size_t child_count = (rsdt->base.length - sizeof(rsdt->base)) / 4;

    for (size_t i = 0; i < child_count; i++)
    {
        auto sdth = mmap_io_ptr_from_phys(struct acpi_sdth *, rsdt->children[i]);

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

struct acpi_madt_record_table acpi_madt_multiple_record(struct acpi_madt *madt, enum acpi_madt_record_type type)
{
    struct acpi_madt_record_table result = {};
    struct acpi_madt_record *current = madt->records;

    while ((uintptr_t)current < (uintptr_t)&madt->base + madt->base.length)
    {
        if (current->type == type)
        {
            result.table[result.count] = current;
            result.count++;
        }

        current = (struct acpi_madt_record *)(((uintptr_t)current) + current->lenght);
    }

    return result;
}

struct acpi_madt *acpi_find_madt(uintptr_t rsdp_address)
{
    auto rsdp = mmap_io_ptr_from_phys(struct acpi_rsdp *, rsdp_address);
    auto rsdt = mmap_io_ptr_from_phys(struct acpi_rsdt *, rsdp->rsdt_address);

    return (struct acpi_madt *)acpi_rsdt_child(rsdt, make_str("APIC"));
}

uint32_t acpi_find_lapic(uintptr_t rsdp_address)
{
    auto madt = acpi_find_madt(rsdp_address);

    return madt->local_apic;
}

struct lapic_record_table acpi_find_lapic_table(uintptr_t rsdp_address)
{
    auto madt = acpi_find_madt(rsdp_address);
    struct acpi_madt_record_table table = acpi_madt_multiple_record(madt, ACPI_MADT_RECORD_LAPIC);
    struct lapic_record_table final =
        {
            .count = table.count,

        };

    for (size_t i = 0; i < table.count; i++)
    {
        final.table[i] = (struct acpi_madt_lapic_record *)table.table[i];
    }
    return final;
}

struct ioapic_record_table acpi_find_ioapic_table(uintptr_t rsdp_address)
{
    auto madt = acpi_find_madt(rsdp_address);
    struct acpi_madt_record_table table = acpi_madt_multiple_record(madt, ACPI_MADT_RECORD_IOAPIC);
    struct ioapic_record_table final =
        {
            .count = table.count,
        };

    for (size_t i = 0; i < table.count; i++)
    {
        final.table[i] = (struct acpi_madt_ioapic_record *)table.table[i];
    }
    return final;
}

struct iso_record_table acpi_find_iso_table(uintptr_t rsdp_address)
{
    auto madt = acpi_find_madt(rsdp_address);
    struct acpi_madt_record_table table = acpi_madt_multiple_record(madt, ACPI_MADT_RECORD_MADT_ISO);
    struct iso_record_table final =
        {
            .count = table.count,
        };

    for (size_t i = 0; i < table.count; i++)
    {
        final.table[i] = (struct acpi_madt_iso_record *)table.table[i];
    }
    return final;
}
