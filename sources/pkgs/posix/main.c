#include <bal/ipc.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <protos/serv/bbus.h>

int br_entry_args()
{
    IpcEv ev;
    br_ev_init(&ev, nullptr, alloc_global());

    Str req = str$("pci");
    BrAddr resp;
    bbus_locate(&ev, BR_ADDR_SUPER, &req, &resp, alloc_global());

    log$("PCI id is {}", resp.id);

    return br_ev_run(&ev);
}
