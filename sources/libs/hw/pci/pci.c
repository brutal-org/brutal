#include <acpi/mcfg.h>
#include <acpi/rsdt.h>
#include <pci/addr.h>
#include <brutal-debug>
#include "config.h"
#include "pci.h"

void pci_init(Pci *pci, Acpi *acpi, Alloc *alloc)
{
    vec_init(&pci->groups, alloc);

    AcpiMcfg *mcfg = (AcpiMcfg *)acpi_rsdt_lookup_first(acpi, ACPI_MCFG_SIG);

    if (!mcfg)
    {
        return;
    }

    for (int i = 0; i < acpi_mcfg_record_count(mcfg); i++)
    {
        AcpiMcfgRecord mcfg_rec = mcfg->records[i];

        PciGroup pci_group = (PciGroup){
            .seg = mcfg_rec.segment_groupe,
            .bus_start = mcfg_rec.bus_start,
            .bus_end = mcfg_rec.bus_end,
        };

        bal_mem_init_pmm(&pci_group.mem, mcfg_rec.address, (mcfg_rec.bus_end - mcfg_rec.bus_start) << PCI_BUS_SHIFT);
        bal_mem_map(&pci_group.mem);

        vec_push(&pci->groups, pci_group);
    }
}

void pci_deinit(Pci *pci)
{
    vec_foreach_v(group, &pci->groups)
    {
        bal_mem_deinit(&group.mem);
    }

    vec_deinit(&pci->groups);
}

Iter pci_iter_group(PciGroup *group, PciAddr addr, IterFn fn, void *ctx);

Iter pci_iter_func(PciGroup *group, PciAddr addr, IterFn fn, void *ctx)
{
    if (fn(&addr, ctx) == ITER_STOP)
    {
        return ITER_STOP;
    }

    PciConfig *config = pci_group_config(group, addr);

    if (config->class == 0x06 && config->subclass == 0x04)
    {
        PciConfigType1 *bridge_config = (PciConfigType1 *)config;
        PciAddr bridge_addr = {
            .seg = group->seg,
            .bus = bridge_config->secondary_bus_number,
        };

        return pci_iter_group(group, bridge_addr, fn, ctx);
    }

    return ITER_CONTINUE;
}

Iter pci_iter_slot(PciGroup *group, PciAddr addr, IterFn fn, void *ctx)
{
    PciConfig *config = pci_group_config(group, addr);

    if (pci_iter_func(group, addr, fn, ctx) == ITER_STOP)
    {
        return ITER_STOP;
    }

    if ((config->header_type & 0x80) == 0)
    {
        return ITER_CONTINUE;
    }

    for (int func = 1; func < 8; func++)
    {
        addr.func = func;
        config = pci_group_config(group, addr);

        if (config->vendor == 0xFFFF)
        {
            continue;
        }

        if (pci_iter_func(group, addr, fn, ctx) == ITER_STOP)
        {
            return ITER_STOP;
        }
    }

    return ITER_CONTINUE;
}

Iter pci_iter_bus(PciGroup *group, PciAddr addr, IterFn fn, void *ctx)
{
    for (int slot = 0; slot < 32; slot++)
    {
        addr.slot = slot;
        PciConfig *config = pci_group_config(group, addr);

        if (config->vendor == 0xFFFF)
        {
            continue;
        }

        if (pci_iter_slot(group, addr, fn, ctx) == ITER_STOP)
        {
            return ITER_STOP;
        }
    }

    return ITER_CONTINUE;
}

Iter pci_iter_group(PciGroup *group, PciAddr addr, IterFn fn, void *ctx)
{
    PciConfig *config = pci_group_config(group, addr);

    if ((config->header_type & 0x80) == 0)
    {
        return pci_iter_bus(group, addr, fn, ctx);
    }

    for (uint16_t func = 0; func < 8; func++)
    {
        addr.func = func;
        config = pci_group_config(group, addr);

        if (config->vendor == 0xFFFF)
        {
            continue;
        }

        if (pci_iter_bus(group, addr, fn, ctx) == ITER_STOP)
        {
            return ITER_STOP;
        }
    }

    return ITER_CONTINUE;
}

Iter pci_iter(Pci *pci, IterFn fn, void *ctx)
{
    vec_foreach_v(group, &pci->groups)
    {
        if (pci_iter_group(&group, (PciAddr){group.seg, 0, 0, 0}, fn, ctx) == ITER_STOP)
        {
            return ITER_STOP;
        }
    }

    return ITER_CONTINUE;
}

PciConfig *pci_config(Pci *pci, PciAddr addr)
{
    vec_foreach_v(group, &pci->groups)
    {
        if (group.seg == addr.seg)
        {
            return pci_group_config(&group, addr);
        }
    }

    return nullptr;
}

PciBarInfo pci_read_bar(Pci *pci, PciAddr addr, int bar)
{
    PciConfig *config = pci_config(pci, addr);

    if (config == nullptr || config->header_type & 0x7f)
    {
        return (PciBarInfo){};
    }

    volatile PciConfigType0 *v = (volatile PciConfigType0 *)config;

    PciBarInfo res = {};
    uint32_t value = v->bars[bar];

    if ((value & 0b111) == 0b110)
    {
        res.type = PCI_BAR_MMIO_64;
        res.base = value & 0xFFFFFFF0;
    }
    else if ((value & 0b111) == 0b000)
    {
        res.type = PCI_BAR_MMIO_32;
        res.base = value & 0xFFFFFFF0;
    }
    else if ((value & 0b111) == 0b001)
    {
        res.type = PCI_BAR_PIO;
        res.base = value & 0xFFFFFFFC;
    }

    res.base = res.base;
    v->bars[bar] = 0xFFFFFFFF;
    res.size = ~(v->bars[bar] & 0xFFFFFFF0) + 1;
    v->bars[bar] = value;

    return res;
}

bool pci_bind_msi(uint8_t cpu, uint8_t vector, PciCapability *pci_msi_cap)
{

    if ((pci_msi_cap->msi.control & MSI_CTRL_64_BIT) == 0)
    {
        panic$("64 bit MSI not supported");
        return false;
    }

    pci_msi_cap->msi.address = (0xfee << 20) | (cpu << 12);
    pci_msi_cap->msi.data = vector;
    pci_msi_cap->msi.control |= MSI_CTRL_ENABLE;
    pci_msi_cap->msi.control &= ~(0b111 << MSI_CTRL_MME_OFF);

    return true;
}
