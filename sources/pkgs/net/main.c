/* megalithicc network server */
#include <bal/hw.h>
#include <brutal/debug.h>
#include "bal/abi/types.h"
#include "interface.h"
#include "pci.h"
#include "rtl8139.h"

int br_entry_handover(Handover *handover)
{
    log$("Hello from the net server!");
    interface_init(handover);

    BrIpcArgs ipc;
    ipc.flags = BR_IPC_RECV | BR_IPC_BLOCK;
    ipc.deadline = BR_DEADLINE_INFINITY;

    while (br_ipc(&ipc) == BR_SUCCESS)
    {
        BrMsg msg = ipc.msg;

        log$("Receive IPC from {}", msg.from);
        if (msg.from == BR_TASK_IRQ && msg.event.type == BR_EVENT_IRQ)
        {
            BrIrq irq = msg.args[0];

            log$("IRQ: {}", irq);
            vec_foreach(v, &interfaces)
            {
                v.driver->handle(v.ctx, irq);
            }

            br_ack(&(BrAckArgs){
                .event = msg.event
            });
        }
        else
        {
            log$("Unknown message!");
        }
    }
    return 0;
}
