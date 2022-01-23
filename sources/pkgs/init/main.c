#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <protos/bbus.h>
#include "init/boot.h"
#include "init/bus.h"

BbusError handle_bbus_locate();

BbusError handle_bbus_locate(
    IpcComponent *self,
    MAYBE_UNUSED BrAddr from,
    Str const *req,
    BrAddr *resp,
    MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = self->ctx;
    BrAddr id = bus_lookup(bus, *req);

    if (id.id == BR_ID_NIL)
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
    .locate = handle_bbus_locate,
};

int br_main(Handover *handover)
{
    boot_splashscreen(handover);

    Bus bus;
    bus_init(&bus, handover, alloc_global());

    IpcComponent self = {};
    ipc_component_init(&self, &bus, alloc_global());

    IpcCap bbus_cap = bbus_locator_provide(&self, &_bbus_vtable);

    bus_start(&bus, str$("acpi"), bal_args_handover(handover));
    bus_start(&bus, str$("pci"), bal_args_handover(handover));
    bus_start(&bus, str$("ps2"), bal_args1(0));
    bus_start(&bus, str$("ahci"), bal_args1(0));

    return ipc_component_run(&self);
}
