#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <protos/bbus.h>
#include "init/boot.h"
#include "init/bus.h"

BbusError handle_bbus_locate(void *self, IpcProto const *req, IpcCap *resp, MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = self;
    IpcCap id = bus_lookup(bus, *req);

    if (id.addr.id == BR_ID_NIL)
    {
        return BBUS_NOT_FOUND;
    }
    else
    {
        *resp = id;
        return BBUS_SUCCESS;
    }
}

static BbusLocatorVTable _bbus_vtable = {
    handle_bbus_locate,
};

int br_main(Handover *handover)
{
    boot_splashscreen(handover);

    Bus bus;
    bus_init(&bus, handover, alloc_global());

    IpcComponent self = {};
    ipc_component_init(&self, alloc_global());

    IpcCap bbus_cap = bbus_locator_provide(&self, &_bbus_vtable, &bus);

    bus_start(&bus, str$("acpi"), &bbus_cap, 1);
    bus_start(&bus, str$("pci"), &bbus_cap, 1);
    bus_start(&bus, str$("ps2"), &bbus_cap, 1);
    bus_start(&bus, str$("ahci"), &bbus_cap, 1);

    return ipc_component_run(&self);
}
