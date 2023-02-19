#include <acpi/base.h>
#include <acpi/rsdt.h>
#include <brutal-debug>

#ifdef __kernel__

void acpi_init(Acpi *acpi, uintptr_t base, uintptr_t rsdp)
{
    *acpi = (Acpi){};

    acpi->base = base;
    acpi->rsdp = (AcpiRsdp *)acpi_phys_to_virt(acpi, rsdp);
    acpi->rsdt = (AcpiRsdt *)acpi_phys_to_virt(acpi, acpi->rsdp->rsdt);
}

#else
#    include <bal/abi.h>

void acpi_init(Acpi *acpi, uintptr_t rsdp)
{
    *acpi = (Acpi){};

    bal_mem_init_pmm(&acpi->mem, 0, GiB(4));
    bal_mem_map(&acpi->mem);

    acpi->rsdp = (AcpiRsdp *)acpi_phys_to_virt(acpi, rsdp);
    acpi->rsdt = (AcpiRsdt *)acpi_phys_to_virt(acpi, acpi->rsdp->rsdt);
}

#endif

void acpi_deinit(Acpi *acpi)
{
#ifdef __kernel__
    (void)acpi;
#else
    bal_mem_deinit(&acpi->mem);
#endif
}

uintptr_t acpi_phys_to_virt(Acpi *acpi, uintptr_t addr)
{
#ifdef __kernel__
    return acpi->base + addr;
#else
    return (uintptr_t)acpi->mem.buf + addr;
#endif
}
