/* megalithicc network server */
#include <brutal/io.h>
#include <brutal/log.h>
#include "pci.h"
#include "rtl8139.h"

int main(void)
{
    log$("Hello from the net server!");
    PciConfigType0 pci_conf = pci_get_network_card(NULL);
    RTL8139Device *rtl8139_dev = rtl8139_init(&pci_conf);
    log$("rtl8139 initialized \\\\o/");

    BrIpcArgs ipc;
    ipc.flags = BR_IPC_RECV | BR_IPC_BLOCK;
    ipc.deadline = BR_DEADLINE_INFINITY;

    while (br_ipc(&ipc) == BR_SUCCESS)
    {
        BrMsg msg = ipc.msg;

        log$("Receive IPC from {}", msg.from);
        if (msg.from == BR_TASK_IRQ)
        {
            BrIrq irq = msg.arg[0];

            log$("IRQ: {}", irq);
            if (irq == rtl8139_dev->int_line)
            {
                br_ack(&(BrAckArgs){
                    .handle = rtl8139_dev->irq_handle,
                });

                rtl8139_handle_irq();
            }
        }
        else
        {
            log$("Unknown message!");
        }
    }
    return 0;
}
