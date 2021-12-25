#include <bal/ipc.h>
#include <brutal/debug.h>
#include <hw/ahci/ahci.h>
#include <hw/ahci/ahci_device.h>
#include <protos/hw/pci.h>
#include <protos/serv/bbus.h>
int br_entry_args(
    MAYBE_UNUSED long arg1,
    MAYBE_UNUSED long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{
    IpcEv ev = {};
    br_ev_init(&ev, nullptr, alloc_global());

    Str req = str$("pci");
    uint64_t resp = 0;
    bbus_locate(&ev, BR_TASK_INIT, &req, &resp, alloc_global());

    PciFindDeviceRequest r = {
        .identifier = {
            .vendor = 0x8086,
            .classcode = 1,
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

    BalMem sample;
    bal_mem_init_size(&sample, 4096);
    int off = 0;

    while (true)
    {

        if (ahci_io_cmd(&dev, sample.obj, off, 1, false) != true)
        {
            panic$("");
        }
        if (ahci_io_cmd(&dev, sample.obj, off, 1, false) != true)
        {
            panic$("");
        }
        if (ahci_io_cmd(&dev, sample.obj, off, 1, false) != true)
        {
            panic$("");
        }
        if (ahci_io_cmd(&dev, sample.obj, off, 1, false) != true)
        {
            panic$("");
        }
        if (ahci_io_cmd(&dev, sample.obj, off, 1, false) != true)
        {
            panic$("");
        }
        for (int i = 0; i < 512; i++)
        {
            if (*(char *)(sample.buf + i) != 0)
                log$("[{}] = {} {#x}", i + off * 512, *(((char *)sample.buf) + i), *(((uint8_t *)sample.buf) + i));
        }

        ((uint8_t *)sample.buf)[0] = 0xff;
        if (ahci_io_cmd(&dev, sample.obj, off, 1, true) != true)
        {
            panic$("");
        }
        if (ahci_io_cmd(&dev, sample.obj, off, 1, false) != true)
        {
            panic$("");
        }

        for (int i = 0; i < 512; i++)
        {
            if (*(char *)(sample.buf + i) != 0)
                log$("[{}] = {} {#x}", i + off * 512, *(((char *)sample.buf) + i), *(((uint8_t *)sample.buf) + i));
        }
        off++;
    }
    while (true)
    {
    }
    return 0;
}
