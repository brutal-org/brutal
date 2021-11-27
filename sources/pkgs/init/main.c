#include <bal/ipc.h>
#include "init/boot.h"
#include "init/bus.h"

int br_entry_handover(Handover *handover)
{
    boot_splashscreen(handover);

    Bus bus;
    bus_init(&bus, handover, alloc_global());

    IpcEv ev = {};
    br_ev_init(&ev, alloc_global());

    bus_start(&bus, str$("posix"), bal_args1(0));
    bus_start(&bus, str$("acpi"), bal_args_handover(handover));
    bus_start(&bus, str$("pci"), bal_args_handover(handover));
    bus_start(&bus, str$("ps2"), bal_args1(0));

    return br_ev_run(&ev);
}
