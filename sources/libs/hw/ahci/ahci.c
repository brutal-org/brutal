#include <ahci/ahci.h>
#include <ahci/device.h>
#include <brutal/debug.h>

static AhciDeviceType ahci_device_type(HbaPort *port)
{
    uint32_t status = port->sata_status;
    uint8_t ipm = (status >> 8) & 0x0f;
    uint8_t det = status & 0x0f;

    if (ipm != HBA_PORT_IPM_ACTIVE || det != HBA_PORT_DET_PRESENT)
    {
        return AHCI_DEVICE_NONE;
    }

    switch (port->signature)
    {
    case HBA_SIG_ATAPI:
        return AHCI_DEVICE_SATAPI;

    case HBA_SIG_SEMB:
        return AHCI_DEVICE_SEMB;

    case HBA_SIG_PM:
        return AHCI_DEVICE_PM;

    case HBA_SIG_ATA:
        return AHCI_DEVICE_SATA;

    case HBA_SIG_UNCONNECTED:
        return AHCI_DEVICE_UNCONNECTED;

    default:
        panic$("Unkown port signature: {#x}", port->signature);
        return AHCI_DEVICE_NONE;
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
        AhciDeviceType port_type = ahci_device_type(port);
        AhciDevice dev = {};

        if (port_type == AHCI_DEVICE_NONE)
        {
            continue;
        }

        ahci_device_init(&dev, port, i);

        port_type = ahci_device_type(port);

        if (port_type == AHCI_DEVICE_SATA)
        {
            log$("Detected device SATA at: {}", i);
        }

        if (port_type == AHCI_DEVICE_PM)
        {
            log$("Detected device PM at: {}", i);
        }

        if (port_type == AHCI_DEVICE_SEMB)
        {
            log$("Detected device SEMB at: {}", i);
        }

        if (port_type == AHCI_DEVICE_SATAPI)
        {
            log$("Detected device SATAPI at: {}", i);
        }

        if (port_type == AHCI_DEVICE_UNCONNECTED)
        {
            log$("Detected device UNCONNECTED at: {}", i);
        }

        vec_push(&ahci->devs, dev);

        curr_port >>= 1;
    }

    if (ahci->devs.len == 0)
    {
        log$("no device founded");
    }
}

void ahci_init(Ahci *ahci, PciBarInfo *bar, Alloc *alloc)
{
    *ahci = (Ahci){};
    BalMem ahci_mem = {};
    vec_init(&ahci->devs, alloc);
    UNWRAP(bal_mem_init_pmm(&ahci_mem, bar->base, bar->size));

    ahci->hba_mem = ahci_mem.buf;
    log$("ahci cap: {#b}", ahci->hba_mem->capability);
    atomic_signal_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_acq_rel);
    ahci->hba_mem->global_host_control |= (uint32_t)1 << 31 | 1 << 1;
    ahci->hba_mem->bios_handoff_control_status |= 1 << 1;
    ahci_init_ports(ahci);
}
