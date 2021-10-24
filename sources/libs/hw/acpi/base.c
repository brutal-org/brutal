#include <acpi/base.h>
#include <acpi/rsdt.h>
#include <brutal/log.h>

#ifndef __kernel__
#    include <bal/helpers.h>
#    include <bal/syscalls.h>
#endif

#ifdef __kernel__

void acpi_init(Acpi *acpi, uintptr_t base, uintptr_t rsdp)
{
    *acpi = (Acpi){};

    acpi->base = base;
    acpi->rsdp = (AcpiRsdp *)acpi_phys_to_virt(acpi, rsdp);
    acpi->rsdt = (AcpiRsdt *)acpi_phys_to_virt(acpi, acpi->rsdp->rsdt);
}

#else

void acpi_init(Acpi *acpi, uintptr_t rsdp)
{
    *acpi = (Acpi){};

    br_mmio_init(&acpi->mmio, 0, GiB(4));

    acpi->rsdp = (AcpiRsdp *)acpi_phys_to_virt(acpi, rsdp);
    acpi->rsdt = (AcpiRsdt *)acpi_phys_to_virt(acpi, acpi->rsdp->rsdt);
}

#endif

void acpi_deinit(Acpi *acpi)
{
#ifdef __kernel__
    (void)acpi;
#else
    br_mmio_deinit(&acpi->mmio);
#endif
}

uintptr_t acpi_phys_to_virt(Acpi *acpi, uintptr_t addr)
{
#ifdef __kernel__
    return acpi->base + addr;
#else
    return acpi->mmio.base + addr;
#endif
}
