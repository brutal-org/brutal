#include <acpi/base.h>
#include <bal/abi.h>
#include <bal/boot.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <pci/pci.h>
#include <protos/hw/pci.h>

typedef struct
{
    PciAddr *result;
    PciIdentifier identifier;
    Pci *pci;
} PciGetDevIterCtx;

static Iter iter_pci_find(void *data, PciGetDevIterCtx *ctx)
{
    PciAddr *addr = (PciAddr *)data;
    Pci *pci = ctx->pci;
    PciConfig *config = pci_config(pci, *addr);

    if (config->classcode == ctx->identifier.classcode &&
        config->subclass == ctx->identifier.subclass &&
        config->vendor == ctx->identifier.vendor)
    {
        *ctx->result = (PciAddr){
            .bus = addr->bus,
            .func = addr->func,
            .seg = addr->seg,
            .slot = addr->slot};

        return ITER_STOP;
    }
    return ITER_CONTINUE;
}

PciError pci_impl_find_device(IpcEv *ev, BrTask task, PciFindDeviceRequest const *req, PciAddr *resp, Alloc *alloc)
{
    UNUSED(task);
    UNUSED(alloc);

    PciGetDevIterCtx ctx = {.result = resp, .identifier = req->identifier, .pci = ev->ctx};

    if (pci_iter(ev->ctx, (IterFn *)iter_pci_find, &ctx) == ITER_CONTINUE)
    {
        return PCI_NOT_FOUND;
    }

    return PCI_SUCCESS;
}

static PciError pci_impl_bar(IpcEv *ev, BrTask task, PciBarRequest const *req, PciBarInfo *resp, Alloc *alloc)
{
    UNUSED(alloc);
    UNUSED(task);

    PciConfig *config = pci_config(ev->ctx, (PciAddr){.bus = req->addr.bus, .func = req->addr.func, .seg = req->addr.seg, .slot = req->addr.slot});

    if (config == nullptr)
    {
        return PCI_INVALID_ADDR;
    }
    config->command |= 1 << 2 | 1 << 4;
    log$("{}", config->command);
    *resp = pci_read_bar(ev->ctx, req->addr, req->num);

    return PCI_SUCCESS;
}

static PciError pci_impl_enable_irq(IpcEv *ev, BrTask task, PciEnableIrqRequest const *req, uint8_t *resp, Alloc *alloc)
{
    UNUSED(alloc);
    UNUSED(task);

    PciConfig *config = pci_config(ev->ctx, (PciAddr){.bus = req->addr.bus, .func = req->addr.func, .seg = req->addr.seg, .slot = req->addr.slot});

    if (config == nullptr)
    {
        return PCI_INVALID_ADDR;
    }

    if ((config->header_type & 0x7f) == 0)
    {
        PciConfigType0 *v = (PciConfigType0 *)config;

        uint8_t cap_offset = v->capabilities_pointer;
        bool founded = false;
        PciCapability *cap = (PciCapability *)((uint8_t *)config + cap_offset);

        while (cap_offset)
        {
            uint8_t value = cap->cap_id;
            cap_offset = cap->next;
            if (value == PCI_CAP_MSI)
            {
                founded = true;
                break;
            }
            cap = (PciCapability *)((uint8_t *)config + cap_offset);
        }

        if (!founded)
        {
            return PCI_NO_IRQ_FOR_DEVICE;
        }

        pci_bind_msi(0, 30, cap);

        *resp = 30;
    }
    else
    {
        PciConfigType1 *v = (PciConfigType1 *)config;
        *resp = v->interrupt_line;
    }
    return PCI_SUCCESS;
}

static PciVTable pci_vtable = {
    pci_impl_find_device,
    pci_impl_bar,
    pci_impl_enable_irq,
};

static Iter iter_pci(void *data, void *ctx)
{
    PciAddr *addr = (PciAddr *)data;
    Pci *pci = (Pci *)ctx;
    PciConfig *config = pci_config(pci, *addr);

    log$("Seg: {} Bus: {} Slot: {} Func: {]", addr->seg, addr->bus, addr->slot, addr->func);
    log$("    VENDOR:{#04x} DEVICE  :{#04x}", config->vendor, config->device);
    log$("    CLASS :{#04x} SUBCLASS:{#04x}", config->classcode, config->subclass);

    if ((config->header_type & 0x7f) == 0)
    {
        PciConfigType0 *v = (PciConfigType0 *)config;
        log$("    Subsytem:  {#04x} VENDOR: {#04x}", v->subsystem_id, v->subsystem_vendor);
        log$("    Irq:       {03}    Pin:    {03}", v->interrupt_line, v->interrupt_pint);

        uint8_t cap_offset = v->capabilities_pointer;
        PciCapability *cap = (PciCapability *)((uint8_t *)config + cap_offset);

        while (cap_offset)
        {
            uint8_t value = cap->cap_id;
            cap_offset = cap->next;
            cap = (PciCapability *)((uint8_t *)config + cap_offset);
            log$("     - {}", cap2str(value));
        }
    }

    return ITER_CONTINUE;
}

int br_entry_handover(Handover *handover)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    Acpi acpi;
    acpi_init(&acpi, handover->rsdp);

    Pci pci;
    pci_init(&pci, &acpi, base$(&heap));

    pci_iter(&pci, iter_pci, &pci);

    IpcEv ev = {};
    ev.ctx = &pci;
    br_ev_init(&ev, &pci, alloc_global());
    pci_impl(&ev, &pci_vtable);

    int res = br_ev_run(&ev);
    pci_deinit(&pci);
    acpi_deinit(&acpi);

    heap_alloc_deinit(&heap);

    return res;
}
