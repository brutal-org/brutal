#include <acpi/base.h>
#include <bal/abi.h>
#include <bal/boot.h>
#include <pci/pci.h>
#include <protos/boot.h>
#include <protos/pci.h>
#include <protos/system.h>
#include <brutal-alloc>
#include <brutal-debug>

typedef struct
{
    PciAddr *result;
    PciIdentifier identifier;
    Pci *pci;
} PciBusQueryCtx;

static Iter pci_bus_query_iter(void *data, PciBusQueryCtx *ctx)
{
    PciAddr *addr = (PciAddr *)data;
    Pci *pci = ctx->pci;
    PciConfig *config = pci_config(pci, *addr);

    if (config->class == ctx->identifier.class &&
        config->subclass == ctx->identifier.subclass &&
        config->vendor == ctx->identifier.vendor)
    {
        *ctx->result = (PciAddr){
            .bus = addr->bus,
            .func = addr->func,
            .seg = addr->seg,
            .slot = addr->slot,
        };

        return ITER_STOP;
    }

    return ITER_CONTINUE;
}

static PciError pci_bus_query_handler(void *ctx, PciIdentifier const *req, PciAddr *resp, Alloc *)
{
    Pci *pci = ctx;
    PciBusQueryCtx query = {.result = resp, .identifier = *req, .pci = pci};

    if (pci_iter(pci, (IterFn *)pci_bus_query_iter, &query) == ITER_CONTINUE)
    {
        return PCI_NOT_FOUND;
    }

    return IPC_SUCCESS;
}

static PciError pci_bus_bar_handler(void *ctx, PciBusBarRequest const *req, PciBarInfo *resp, Alloc *)
{
    Pci *pci = ctx;
    PciConfig *config = pci_config(pci, req->addr);

    if (config == nullptr)
    {
        return PCI_INVALID_ADDR;
    }

    config->command |= 1 << 2 | 1 << 4;
    *resp = pci_read_bar(pci, req->addr, req->num);

    return IPC_SUCCESS;
}

static PciError pci_bus_enable_irq_handler(void *ctx, PciBusEnableIrqRequest const *req, uint8_t *resp, MAYBE_UNUSED Alloc *alloc)
{
    Pci *pci = ctx;
    PciConfig *config = pci_config(pci, req->addr);

    if (config == nullptr)
    {
        return PCI_INVALID_ADDR;
    }

    if ((config->header_type & 0x7f) == 0)
    {
        PciConfigType0 *v = (PciConfigType0 *)config;

        uint8_t cap_offset = v->capabilities_pointer;
        bool found = false;
        PciCapability *cap = (PciCapability *)((uint8_t *)config + cap_offset);

        while (cap_offset)
        {
            uint8_t value = cap->cap_id;
            cap_offset = cap->next;
            if (value == PCI_CAP_MSI)
            {
                found = true;
                break;
            }
            cap = (PciCapability *)((uint8_t *)config + cap_offset);
        }

        if (!found)
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

    return IPC_SUCCESS;
}

static PciBusVTable pci_bus_vtable = {
    pci_bus_query_handler,
    pci_bus_bar_handler,
    pci_bus_enable_irq_handler,
};

static Iter iter_pci(void *data, void *ctx)
{
    PciAddr *addr = (PciAddr *)data;
    Pci *pci = (Pci *)ctx;
    PciConfig *config = pci_config(pci, *addr);

    log$("Seg: {} Bus: {} Slot: {} Func: {]", addr->seg, addr->bus, addr->slot, addr->func);
    log$("    VENDOR:{#04x} DEVICE  :{#04x}", config->vendor, config->device);
    log$("    CLASS :{#04x} SUBCLASS:{#04x}", config->class, config->subclass);

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

int ipc_component_main(IpcComponent *self)
{
    IpcCap boot_infos = ipc_component_require(self, IPC_BOOT_INFO_PROTO);
    IpcCap system_server = ipc_component_require(self, IPC_SYSTEM_SERVER_PROTO);

    uintptr_t rsdp = 0;
    if (boot_info_rsdp_rpc(self, boot_infos, &rsdp, alloc_global()) != IPC_SUCCESS)
    {
        panic$("No rsdp found!");
    }

    Acpi acpi;
    acpi_init(&acpi, rsdp);

    Pci pci;
    pci_init(&pci, &acpi, alloc_global());
    pci_iter(&pci, iter_pci, &pci);

    IpcObject obj = {};
    ipc_object_init(&obj, ipc_self(), alloc_global());

    IpcCap cap = pci_bus_provide(&obj, &pci_bus_vtable);
    system_server_expose_rpc(self, system_server, &cap, alloc_global());

    return ipc_component_run(self);
}
