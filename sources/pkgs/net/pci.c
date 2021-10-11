#include "pci.h"
#include <pci/config.h>
#include <handover/handover.h>

PciConfigType0 pci_get_network_card(Handover *handover)
{
    (void)handover;
    PciConfigType0 conf = {
        .bars = { {0xC001}, {0x0}, {0x0}, {0x0}, {0x0}, {0x0} },
        .interrupt_line = 0xb
    };
    // FIXME : todo
    return conf;
}

uint32_t pci_get_base_io(PciConfigType0 *conf)
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
