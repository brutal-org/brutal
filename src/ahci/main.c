#include <ahci/ahci.h>
#include <ahci/device.h>
#include <protos/pci.h>
#include <brutal-alloc>
#include <brutal-debug>

int ipc_component_main(IpcComponent *self)
{
    IpcCap pci_bus = ipc_component_require(self, IPC_PCI_BUS_PROTO);

    PciIdentifier id = {
        .vendor = 0x8086,
        .class = 1,
        .subclass = 6,
    };

    PciAddr ahci_device;
    if (pci_bus_query_rpc(self, pci_bus, &id, &ahci_device, alloc_global()) != IPC_SUCCESS)
    {
        log$("No AHCI controller found!");
        return 0;
    }

    log$("AHCI controller found on bus: {} func: {} seg: {} slot: {}", ahci_device.bus, ahci_device.func, ahci_device.seg, ahci_device.slot);

    PciBarInfo pci_bar = {};
    pci_bus_bar_rpc(self, pci_bus, &(PciBusBarRequest){.addr = ahci_device, .num = 5}, &pci_bar, alloc_global());

    log$("bar[5] {#x}-{#x} type: {}", pci_bar.base, pci_bar.base + pci_bar.size, pci_bar.type);

    Ahci ahci;
    ahci_init(&ahci, &pci_bar, alloc_global());
    AhciDevice dev = ahci.devs.data[0];

    UNUSED(dev);

    return ipc_component_run(self);
}
