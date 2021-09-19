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
    HeapAlloc heap;
    heap_alloc_init(&heap);

    Acpi acpi;
    acpi_init(&acpi, handover->rsdp);

    Pci pci;
    pci_init(&pci, &acpi, base$(&heap));

    pci_iter(&pci, iter_pci, &pci);

    pci_deinit(&pci);
    acpi_deinit(&acpi);

    heap_alloc_deinit(&heap);

    return 0;
}
