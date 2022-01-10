#include <ahci/ahci.h>
#include <ahci/device.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <ipc/ipc.h>
#include <protos/hw/pci.h>
#include <protos/serv/bbus.h>

int br_entry_args(
    MAYBE_UNUSED long arg1,
    MAYBE_UNUSED long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{
    IpcComponent ev = {};
    ipc_component_init(&ev, nullptr, alloc_global());

    Str req = str$("pci");
    BrAddr resp;
    while (bbus_locate(&ev, BR_ADDR_SUPER, &req, &resp, alloc_global()) == BBUS_NOT_FOUND)
        ;

    PciFindDeviceRequest r = {
        .identifier = {
            .vendor = 0x8086,
            .class = 1,
            .subclass = 6,
        }};

    PciAddr res;
    if (pci_find_device(&ev, resp, &r, &res, alloc_global()) == PCI_NOT_FOUND)
    {
        log$("no ahci found !");
        return 0;
    }

    log$("ahci: bus: {} func: {} seg: {} slot: {}", res.bus, res.func, res.seg, res.slot);

    PciBarInfo b = {};
    PciBarRequest bar_req = {.addr = res, .num = 5};
    pci_bar(&ev, resp, &bar_req, &b, alloc_global());

    log$("ahci: bar[5] {#x}-{#x} type: {}", b.base, b.base + b.size, b.type);
    Ahci ahci;
    ahci_init(&ahci, &b, alloc_global());
    AhciDevice dev = ahci.devs.data[0];

    UNUSED(dev);

    return 0;
}
