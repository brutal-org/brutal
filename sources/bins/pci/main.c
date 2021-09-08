#include <acpi/acpi.h>
#include <brutal/alloc.h>
#include <brutal/log.h>
#include <handover/handover.h>
#include <pci/pci.h>

static Iter iter_pci(void *data, void *ctx)
{
    PciAddr *addr = (PciAddr *)data;
    Pci *pci = (Pci *)ctx;
    PciConfig *config = pci_config(pci, *addr);

    log$("VENDOR:{#04x} DEVICE:{#04x}", config->vendor, config->device);

    return ITER_CONTINUE;
}

int br_entry_handover(Handover *handover)
{
    Acpi acpi;
    acpi_init(&acpi, handover->rsdp);

    Pci pci;
    pci_init(&pci, &acpi, alloc_global());

    pci_iter(&pci, iter_pci, &pci);

    pci_deinit(&pci);
    acpi_deinit(&acpi);

    return 0;
}
