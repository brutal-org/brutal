#include <bal/ipc.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <protos/serv/bbus.h>
#include "init/boot.h"
#include "init/bus.h"

BbusError handle_bbus_locate(
    IpcEv *ev,
    MAYBE_UNUSED BrAddr from,
    Str const *req,
    BrAddr *resp,
    MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = ev->ctx;
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

static BbusVTable _bbus_vtable = {
    .locate = handle_bbus_locate,
};

int br_entry_handover(Handover *handover)
{
    boot_splashscreen(handover);

    Bus bus;
    bus_init(&bus, handover, alloc_global());

    IpcEv ev = {};
    br_ev_init(&ev, &bus, alloc_global());

    bus_start(&bus, str$("posix"), bal_args1(0));
    bus_start(&bus, str$("acpi"), bal_args_handover(handover));
    bus_start(&bus, str$("pci"), bal_args_handover(handover));
    bus_start(&bus, str$("ps2"), bal_args1(0));
    bus_start(&bus, str$("ahci"), bal_args1(0));

    bbus_provide(&ev, &_bbus_vtable);

    return br_ev_run(&ev);
}
