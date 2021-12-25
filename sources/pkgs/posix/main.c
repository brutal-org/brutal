#include <bal/ipc.h>
#include <brutal/debug.h>
#include <protos/serv/bbus.h>

int br_entry_args()
{
    IpcEv ev;
    br_ev_init(&ev, nullptr, alloc_global());

    Str req = str$("pci");
    uint64_t resp = 0;
    bbus_locate(&ev, BR_ID_SUPER, &req, &resp, alloc_global());

    log$("PCI id is {}", resp);

    return br_ev_run(&ev);
}
