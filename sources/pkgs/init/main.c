#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <protos/boot.h>
#include <protos/bus.h>
#include <protos/event.h>
#include <protos/hw.h>
#include <protos/pci.h>
#include <protos/wm.h>
#include "init/boot.h"
#include "init/bus.h"

/* --- BBus Server Protocol ------------------------------------------------- */

static BusError bus_server_expose_handler(void *self, IpcCap const *req, void *, MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = self;
    bus_expose(bus, *req);
    return IPC_SUCCESS;
}

static BusError bus_server_consume_handler(void *self, IpcProto const *req, IpcCap *resp, MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = self;
    *resp = bus_consume(bus, *req);

    if (resp->addr.id == BR_ID_NIL)
    {
        return BUS_NOT_FOUND;
    }

    return IPC_SUCCESS;
}

static BusServerVTable _bus_server_vtable = {
    bus_server_expose_handler,
    bus_server_consume_handler,
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

HwError hw_display_surface_handler(void *self, void *, Surface *resp, Alloc *)
{
    Handover *handover = self;
    HandoverFramebuffer const *framebuffer = &handover->framebuffer;

    size_t fb_size = align_up$(framebuffer->height * framebuffer->pitch, MEM_PAGE_SIZE);

    BrCreateArgs fb_mem = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .addr = framebuffer->addr,
            .size = fb_size,
            .flags = BR_MEM_PMM,
        },
    };

    br_create(&fb_mem);

    *resp = (Surface){
        .mode = {
            .width = framebuffer->width,
            .height = framebuffer->height,
            .pitch = framebuffer->pitch,
            .format = GFX_FMT_BGRA8888,
        },

        .handle = fb_mem.handle,
    };

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
    boot_splashscreen(handover);

    Bus bus;
    bus_init(&bus, handover, alloc_global());

    IpcComponent self = {};
    ipc_component_init(&self, alloc_global());

    IpcCap bus_server_cap = bus_server_provide(&self, &_bus_server_vtable, &bus);
    bus_expose(&bus, bus_server_cap);

    IpcCap boot_info_cap = boot_info_provide(&self, &_bus_info_vtable, handover);
    bus_expose(&bus, boot_info_cap);

    IpcCap hw_display_cap = hw_display_provide(&self, &_hw_display_vtable, handover);
    bus_expose(&bus, hw_display_cap);

    Unit acpi_unit;
    unit_init(&acpi_unit, str$("acpi"), alloc_global());
    unit_consume(&acpi_unit, IPC_BUS_SERVER_PROTO);
    unit_consume(&acpi_unit, IPC_BOOT_INFO_PROTO);
    bus_activate(&bus, &acpi_unit);

    Unit pci_unit;
    unit_init(&pci_unit, str$("pci"), alloc_global());
    unit_consume(&pci_unit, IPC_BUS_SERVER_PROTO);
    unit_consume(&pci_unit, IPC_BOOT_INFO_PROTO);
    bus_activate(&bus, &pci_unit);

    Unit ps2_unit;
    unit_init(&ps2_unit, str$("ps2"), alloc_global());
    unit_consume(&ps2_unit, IPC_BUS_SERVER_PROTO);
    unit_consume(&ps2_unit, IPC_EVENT_SINK_PROTO);
    bus_activate(&bus, &ps2_unit);

    /*
    Unit ahci_unit;
    unit_init(&ahci_unit, str$("ahci"), alloc_global());
    unit_consume(&ahci_unit, IPC_BUS_SERVER_PROTO);
    unit_consume(&ahci_unit, IPC_PCI_BUS_PROTO);
    bus_activate(&bus, &ahci_unit);
*/

    Unit wm_unit;
    unit_init(&wm_unit, str$("wm"), alloc_global());
    unit_consume(&wm_unit, IPC_BUS_SERVER_PROTO);
    unit_consume(&wm_unit, IPC_HW_DISPLAY_PROTO);
    bus_activate(&bus, &wm_unit);

    Unit about_unit;
    unit_init(&about_unit, str$("about"), alloc_global());
    unit_consume(&about_unit, IPC_BUS_SERVER_PROTO);
    unit_consume(&about_unit, IPC_WM_SERVER_PROTO);
    bus_activate(&bus, &about_unit);

    return ipc_component_run(&self);
}
