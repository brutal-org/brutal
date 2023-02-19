#include <pci/group.h>

PciConfig *pci_group_config(PciGroup *group, PciAddr addr)
{
    if (group->seg == addr.seg &&
        group->bus_start <= addr.bus &&
        group->bus_end > addr.bus)
    {
        uint16_t local_bus = (addr.bus - group->bus_start);

        size_t offset =
            local_bus << PCI_BUS_SHIFT |
            addr.slot << PCI_SLOT_SHIFT |
            addr.func << PCI_FUNC_SHIFT;

        return (PciConfig *)(group->mem.buf + offset);
    }

    return nullptr;
}
