#include <ahci/ahci.h>
#include <ahci/device.h>
#include <brutal-debug>

static HbaPortSig ahci_device_type(HbaPort *port)
{
    uint32_t status = port->sata_status;
    uint8_t ipm = (status >> 8) & 0x0f;
    uint8_t det = status & 0x0f;

    if (ipm != HBA_PORT_IPM_ACTIVE ||
        det != HBA_PORT_DET_PRESENT)
    {
        return HBA_SIG_UNCONNECTED;
    }

    return port->signature;
}

Str ahci_port_sig_to_str(HbaPortSig type)
{
    switch (type)
    {
    case HBA_SIG_ATA:
        return str$("SATA");
    case HBA_SIG_PM:
        return str$("PM");
    case HBA_SIG_SEMB:
        return str$("SEMB");
    case HBA_SIG_ATAPI:
        return str$("SATAPI");
    default:
        return str$("UNCONNECTED");
    }
}

static void ahci_init_ports(Ahci *ahci)
{
    volatile uint32_t curr_port = ahci->hba_mem->port_implemented;

    for (int i = 0; i < 32; i++)
    {
        if (!(curr_port & 1))
        {
            continue;
        }

        HbaPort *port = &ahci->hba_mem->ports[i];
        HbaPortSig port_type = ahci_device_type(port);
        AhciDevice dev = {};

        if (port_type == HBA_SIG_UNCONNECTED)
        {
            continue;
        }

        ahci_device_init(&dev, port, i);

        port_type = ahci_device_type(port);

        log$("Detected device {} at: {}", ahci_port_sig_to_str(port_type), i);

        vec_push(&ahci->devs, dev);

        curr_port >>= 1;
    }

    if (ahci->devs.len == 0)
    {
        log$("no device found");
    }
}

void ahci_init(Ahci *ahci, PciBarInfo *bar, Alloc *alloc)
{
    *ahci = (Ahci){};
    BalMem ahci_mem = {};
    vec_init(&ahci->devs, alloc);
    UNWRAP(bal_mem_init_pmm(&ahci_mem, bar->base, bar->size));
    bal_mem_map(&ahci_mem);

    ahci->hba_mem = ahci_mem.buf;
    log$("ahci cap: {#b}", ahci->hba_mem->capability);
    atomic_signal_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_acq_rel);
    ahci->hba_mem->global_host_control |= (uint32_t)1 << 31 | 1 << 1;
    ahci->hba_mem->bios_handoff_control_status |= 1 << 1;
    ahci_init_ports(ahci);
}
