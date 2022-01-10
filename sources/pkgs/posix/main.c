#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <ipc/ipc.h>
#include <protos/serv/bbus.h>

int br_entry_args()
{
    IpcComponent ev;
    ipc_component_init(&ev, nullptr, alloc_global());

    Str req = str$("pci");
    BrAddr resp;
    bbus_locate(&ev, BR_ADDR_SUPER, &req, &resp, alloc_global());

    log$("PCI id is {}", resp.id);

    return ipc_component_run(&ev);
}
