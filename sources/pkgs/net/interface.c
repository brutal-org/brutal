#include <brutal/alloc.h>
#include <brutal/ds.h>
#include <brutal/log.h>
#include "interface.h"
#include "net/pci.h"

VecInterface interfaces;

void interface_init(Handover *handover)
{
    log$("Initialize interfaces");
    vec_init(&interfaces, alloc_global());

    pci_get_network_cards(handover);

    size_t count = 0;
    vec_foreach(v, &interfaces)
    {
        uint8_t *mac = v.driver->get_mac(v.ctx);
        log$("eth{} mac: {02x}:{02x}:{02x}:{02x}:{02x}:{02x}", count, mac[0], mac[1],
             mac[2], mac[3], mac[4], mac[5]);
        count++;
    }

    log$("Interfaces initialized \\\\o/");
}
