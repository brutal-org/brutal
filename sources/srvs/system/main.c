#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <protos/boot.h>
#include <protos/event.h>
#include <protos/hw.h>
#include <protos/pci.h>
#include <protos/system.h>
#include <protos/window.h>
#include "system/bus.h"

/* --- System Server Protocol ----------------------------------------------- */

static SystemError system_server_expose_handler(void *self, IpcCap const *req, void *, MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = self;
    bus_expose(bus, *req);
    return IPC_SUCCESS;
}

static SystemError system_server_consume_handler(void *self, IpcProto const *req, IpcCap *resp, MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = self;
    *resp = bus_consume(bus, *req);

    if (resp->addr.id == BR_ID_NIL)
    {
        return SYSTEM_NOT_FOUND;
    }

    return IPC_SUCCESS;
}

static SystemServerVTable _system_server_vtable = {
    system_server_expose_handler,
    system_server_consume_handler,
};

/* --- Boot Info Protocol --------------------------------------------------- */

static BootError boot_info_rsdp_handler(void *self, void *, uintptr_t *resp, Alloc *)
{
    Handover *handover = self;
    *resp = handover->rsdp;
    return IPC_SUCCESS;
}

static BootInfoVTable _bus_info_vtable = {
    boot_info_rsdp_handler,
};

/* --- Hw Display Protocol -------------------------------------------------- */

HwError hw_display_surface_handler(void *self, void *, BalFb *resp, Alloc *)
{
    Handover *handover = self;
    HandoverFramebuffer const *framebuffer = &handover->framebuffer;

    size_t fb_size = align_up$(framebuffer->height * framebuffer->pitch, MEM_PAGE_SIZE);

    bal_fb_init_pmm(
        resp,
        framebuffer->addr,
        fb_size,
        framebuffer->width,
        framebuffer->height,
        framebuffer->pitch,
        GFX_FMT_BGRA8888);

    return IPC_SUCCESS;
}

static HwDisplayVTable _hw_display_vtable = {
    nullptr,
    nullptr,
    hw_display_surface_handler,
};

/* --- Entry Point ---------------------------------------------------------- */

int br_main(Handover *handover)
{
    Bus bus;
    bus_init(&bus, handover, alloc_global());

    IpcComponent self = {};
    ipc_component_init(&self, alloc_global());

    IpcCap system_server_cap = system_server_provide(&self, &_system_server_vtable, &bus);
    bus_expose(&bus, system_server_cap);

    IpcCap boot_info_cap = boot_info_provide(&self, &_bus_info_vtable, handover);
    bus_expose(&bus, boot_info_cap);

    IpcCap hw_display_cap = hw_display_provide(&self, &_hw_display_vtable, handover);
    bus_expose(&bus, hw_display_cap);

    Unit acpi_unit;
    unit_init(&acpi_unit, str$("acpi"), alloc_global());
    unit_consume(&acpi_unit, IPC_SYSTEM_SERVER_PROTO);
    unit_consume(&acpi_unit, IPC_BOOT_INFO_PROTO);
    bus_activate(&bus, &acpi_unit);

    Unit pci_unit;
    unit_init(&pci_unit, str$("pci"), alloc_global());
    unit_consume(&pci_unit, IPC_SYSTEM_SERVER_PROTO);
    unit_consume(&pci_unit, IPC_BOOT_INFO_PROTO);
    bus_activate(&bus, &pci_unit);

    Unit ps2_unit;
    unit_init(&ps2_unit, str$("ps2"), alloc_global());
    unit_consume(&ps2_unit, IPC_SYSTEM_SERVER_PROTO);
    unit_consume(&ps2_unit, IPC_EVENT_SINK_PROTO);
    bus_activate(&bus, &ps2_unit);

    Unit ahci_unit;
    unit_init(&ahci_unit, str$("ahci"), alloc_global());
    unit_consume(&ahci_unit, IPC_SYSTEM_SERVER_PROTO);
    unit_consume(&ahci_unit, IPC_PCI_BUS_PROTO);
    bus_activate(&bus, &ahci_unit);

    Unit window_unit;
    unit_init(&window_unit, str$("window"), alloc_global());
    unit_consume(&window_unit, IPC_SYSTEM_SERVER_PROTO);
    unit_consume(&window_unit, IPC_HW_DISPLAY_PROTO);
    bus_activate(&bus, &window_unit);

    Unit about_unit;
    unit_init(&about_unit, str$("about"), alloc_global());
    unit_consume(&about_unit, IPC_SYSTEM_SERVER_PROTO);
    unit_consume(&about_unit, IPC_WINDOW_SERVER_PROTO);
    bus_activate(&bus, &about_unit);

    Unit panel_unit;
    unit_init(&panel_unit, str$("panel"), alloc_global());
    unit_consume(&panel_unit, IPC_SYSTEM_SERVER_PROTO);
    unit_consume(&panel_unit, IPC_WINDOW_SERVER_PROTO);
    bus_activate(&bus, &panel_unit);

    return ipc_component_run(&self);
}
