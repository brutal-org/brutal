#include <acpi/mcfg.h>
#include <acpi/rsdt.h>
#include <brutal/debug.h>
#include <pci/addr.h>
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

    if (config->classcode == 0x06 && config->subclass == 0x04)
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

PciBarInfo pci_get_bar(Pci *pci, PciAddr addr, int bar)
{
    PciBarInfo res = {};

    PciConfig *config = pci_config(pci, addr);
    volatile uint32_t *bar_ptr = 0;
    if ((config->header_type & 0x7f) == 0)
    {
        PciConfigType0 *v = (PciConfigType0 *)config;
        bar_ptr = &v->bars[bar];
    }
    uint32_t value = *bar_ptr;
    uint64_t base = 0;

    if ((value & 0b0111) == 0b0110)
    {
        res.type = PCI_BAR_MMIO_64;
        base = value & 0xFFFFFFF0;
        //   base = bar_ptr[1] & 0xFFFFFFF0; FIXME: check specification
    }
    else if ((value & 0b0111) == 0b0000)
    {
        res.type = PCI_BAR_MMIO_32;
        base = value & 0xFFFFFFF0;
    }
    else if ((value & 0b0111) == 0b0001)
    {
        res.type = PCI_BAR_PIO;
        base = value & 0xFFFFFFFC;
    }

    res.base = base;

    *bar_ptr = 0xFFFFFFFF;

    res.size = ~(*bar_ptr & 0xFFFFFFF0) + 1;

    *bar_ptr = value;

    return res;
}
