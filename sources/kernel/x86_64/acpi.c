#include <brutal/log.h>
#include <brutal/text.h>
#include "kernel/mmap.h"
#include "kernel/x86_64/acpi.h"

struct acpi_sdth *acpi_rsdt_child(struct acpi_rsdt *rsdt, Str signature)
{
    size_t child_count = (rsdt->base.length - sizeof(rsdt->base)) / 4;

    for (size_t i = 0; i < child_count; i++)
    {
        auto sdth = mmap_io_ptr_from_phys(struct acpi_sdth *, rsdt->children[i]);

        if (str_eq(signature, str$_n(4, sdth->signature)))
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

        current = (struct acpi_madt_record *)(((uintptr_t)current) + current->len);
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

        current = (struct acpi_madt_record *)(((uintptr_t)current) + current->len);
    }

    return result;
}

struct acpi_madt *acpi_find_madt(uintptr_t rsdp_address)
{
    auto rsdp = mmap_io_ptr_from_phys(struct acpi_rsdp *, rsdp_address);
    auto rsdt = mmap_io_ptr_from_phys(struct acpi_rsdt *, rsdp->rsdt_address);

    return (struct acpi_madt *)acpi_rsdt_child(rsdt, str$("APIC"));
}

struct acpi_hpet *acpi_find_hpet(uintptr_t rsdp_address)
{
    auto rsdp = mmap_io_ptr_from_phys(struct acpi_rsdp *, rsdp_address);
    auto rsdt = mmap_io_ptr_from_phys(struct acpi_rsdt *, rsdp->rsdt_address);

    return (struct acpi_hpet *)acpi_rsdt_child(rsdt, str$("HPET"));
}

uint32_t acpi_find_lapic(uintptr_t rsdp_address)
{
    auto madt = acpi_find_madt(rsdp_address);

    return madt->local_apic;
}

#define ACPI_FIND_TABLE_GENERATOR(table_type, record_type, madt_record, name)               \
    struct table_type name(uintptr_t rsdp_address)                                          \
    {                                                                                       \
        auto madt = acpi_find_madt(rsdp_address);                                           \
        struct acpi_madt_record_table table = acpi_madt_multiple_record(madt, madt_record); \
        struct table_type final =                                                           \
            {                                                                               \
                .count = table.count,                                                       \
            };                                                                              \
                                                                                            \
        for (size_t i = 0; i < table.count; i++)                                            \
        {                                                                                   \
            final.table[i] = (struct record_type *)table.table[i];                          \
        }                                                                                   \
                                                                                            \
        return final;                                                                       \
    }

ACPI_FIND_TABLE_GENERATOR(lapic_record_table, acpi_madt_lapic_record, ACPI_MADT_RECORD_LAPIC, acpi_find_lapic_table);
ACPI_FIND_TABLE_GENERATOR(ioapic_record_table, acpi_madt_ioapic_record, ACPI_MADT_RECORD_IOAPIC, acpi_find_ioapic_table);
ACPI_FIND_TABLE_GENERATOR(iso_record_table, acpi_madt_iso_record, ACPI_MADT_RECORD_MADT_ISO, acpi_find_iso_table);
