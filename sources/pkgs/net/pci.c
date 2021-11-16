#include <acpi/base.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <pci/pci.h>
#include "pci.h"
#include "driver.h"
#include "interface.h"

uint32_t pci_get_io_base(PciConfigType0 *conf)
{
    for (int i = 0; i < 6; i++)
    {
        if (conf->bars[i].address & 0x1)
        {
            return conf->bars[i].address & ~0x1;
        }
    }

    return 0x0;
}

static Iter pci_iter_net(void *data, void *ctx)
{
    PciAddr *addr = (PciAddr *)data;
    Pci *pci = (Pci *)ctx;
    PciConfig *config = pci_config(pci, *addr);

    for (int i = 0; nic_device_list[i].driver; i++)
    {
        if (config->vendor == nic_device_list[i].vendor &&
                config->device == nic_device_list[i].device &&
                (config->header_type & 0x7f) == 0)
        {
            log$("Found network card: {#04x} {#04x}", config->vendor, config->device);
            PciConfigType0 *v = (PciConfigType0 *)config;

            Interface interface;
            interface.driver = nic_device_list[i].driver;
            interface.ctx = interface.driver->init(v, v->interrupt_line);
            vec_push(&interfaces, interface);
        }
    }
    return ITER_CONTINUE;
}

void pci_get_network_cards(Handover *handover)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    Acpi acpi;
    acpi_init(&acpi, handover->rsdp);

    Pci pci;
    pci_init(&pci, &acpi, base$(&heap));

    pci_iter(&pci, pci_iter_net, &pci);

    pci_deinit(&pci);
    acpi_deinit(&acpi);

    heap_alloc_deinit(&heap);
}
