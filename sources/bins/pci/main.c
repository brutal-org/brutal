#include <acpi/base.h>
#include <bal/abi.h>
#include <brutal/alloc.h>
#include <brutal/log.h>
#include <pci/pci.h>

static Iter iter_pci(void *data, void *ctx)
{
    PciAddr *addr = (PciAddr *)data;
    Pci *pci = (Pci *)ctx;
    PciConfig *config = pci_config(pci, *addr);

    log$("Seg: {} Bus: {} Slot: {} Func: {]", addr->seg, addr->bus, addr->slot, addr->func);
    log$("    VENDOR:{#04x} DEVICE:{#04x}", config->vendor, config->device);

    if ((config->header_type & 0x7f) == 0)
    {
        PciConfigType0 *v = (PciConfigType0 *)config;
        log$("    Subsytem:  {#04x} VENDOR: {#04x}", v->subsystem_id, v->subsystem_vendor);
    }

    return ITER_CONTINUE;
}

int br_entry_handover(Handover *handover)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

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
