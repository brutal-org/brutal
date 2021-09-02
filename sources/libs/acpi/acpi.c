#include <acpi/acpi.h>
#include <acpi/rsdt.h>

void acpi_init(Acpi *acpi, uintptr_t base, uintptr_t rsdp)
{
    acpi->base = base;
    acpi->rsdp = (AcpiRsdp *)acpi_phys_to_virt(acpi, rsdp);
    acpi->rsdt = (AcpiRsdt *)acpi_phys_to_virt(acpi, acpi->rsdp->rsdt);
}

uintptr_t acpi_phys_to_virt(Acpi *acpi, uintptr_t addr)
{
    return acpi->base + addr;
}
