#include <bal/task.h>
#include <brutal-debug>
#include "system/bus.h"

void bus_init(Bus *bus, Alloc *alloc)
{
    *bus = (Bus){};

    ipc_object_init(base$(bus), ipc_self(), alloc);

    vec_init(&bus->caps, alloc);
    vec_init(&bus->units, alloc);
}

void bus_deinit(Bus *bus)
{
    vec_deinit(&bus->caps);
    vec_deinit(&bus->units);

    ipc_object_deinit(base$(bus));
}

void bus_expose(Bus *self, IpcCap cap)
{
    log$("Exposing capability '{x}'...", cap.proto);

    vec_push(&self->caps, cap);

    vec_foreach(unit, &self->units)
    {
        unit_provide(unit, cap);

        if (unit->state == UNIT_READY)
        {
            unit_start(unit);
        }
    }
}

IpcCap bus_consume(Bus *self, IpcProto proto)
{
    vec_foreach_v(cap, &self->caps)
    {
        if (cap.proto == proto)
        {
            return cap;
        }
    }

    return (IpcCap){};
}

void bus_activate(Bus *self, Unit *unit)
{
    log$("Activating service '{case-pascal}'...", unit->name);

    vec_foreach(cap, &self->caps)
    {
        unit_provide(unit, *cap);

        if (unit->state == UNIT_READY)
        {
            unit_start(unit);
        }
    }

    vec_push(&self->units, *unit);
}
