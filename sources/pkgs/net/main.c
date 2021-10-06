/* megalithicc network server */
#include <brutal/io.h>
#include <brutal/log.h>
#include <stdint.h>
#include <syscalls/helpers.h>
#include <syscalls/syscalls.h>
#include "brutal/mem/const.h"
#include "syscalls/io.h"
#include "syscalls/types.h"

/* PCI ---------------------------------------------------------------------- */

/* RTL8139 driver ----------------------------------------------------------- */

/* from: RTL8139D DataSheet p10-11 */
#define RTL8139_IDR0_REG 0x0
#define RTL8139_IDR4_REG 0x4
#define RTL8139_RBSTART_REG 0x30
#define RTL8139_CR_REG 0x37
#define RTL8139_IMR_REG 0x3C
#define RTL8139_ISR_REG 0x3E
#define RTL8139_RCR_REG 0x44
#define RTL8139_CONFIG1_REG 0x52

#define RTL8139_CR_TE 0x4 /* transmitter enable */
#define RTL8139_CR_RE 0x8 /* receiver enable */

#define RTL8139_IMR_ROK 0x1
#define RTL8139_IMR_TOK 0x4

#define RTL8139_ISR_ROK 0x1
#define RTL8139_ISR_RER 0x2
#define RTL8139_ISR_TOK 0x4
#define RTL8139_ISR_TER 0x8

#define RTL8139_RCR_AAP 0x1  /* Accept All Packets */
#define RTL8139_RCR_APM 0x2  /* Accept Physical Match packets */
#define RTL8139_RCR_AM 0x4   /* Accept Multicast packets */
#define RTL8139_RCR_AB 0x8   /* Accept Broadcast packets */
#define RTL8139_RCR_AR 0x10  /* Accept Runt */
#define RTL8139_RCR_AER 0x20 /* Accept Error packet */
#define RTL8139_RCR_WRAP 0x80

#define RTL8139_RCR_ACCEPT_ALL RTL8139_RCR_AAP |     \
                                     RTL8139_RCR_APM | \
                                     RTL8139_RCR_AM |  \
                                     RTL8139_RCR_AB | \
                                     RTL8139_RCR_AR | \
                                     RTL8139_RCR_AER

#define RTL8139_CONFIG1_LWACT 0x10

typedef struct
{
    uint32_t io_base;
    uint8_t mac[6];
    uint8_t *rx_buffer;
    BrHandle irq_handle;
} RTL8139Device;

void rtl8139_read_mac(RTL8139Device *dev)
{
    uint32_t mac1 = br_in32(dev->io_base + RTL8139_IDR0_REG);
    uint16_t mac2 = br_in16(dev->io_base + RTL8139_IDR4_REG);
    dev->mac[0] = mac1 >> 0;
    dev->mac[1] = mac1 >> 8;
    dev->mac[2] = mac1 >> 16;
    dev->mac[3] = mac1 >> 24;
    dev->mac[4] = mac2 >> 0;
    dev->mac[5] = mac2 >> 8;
}

void rtl8139_init_rx(RTL8139Device *dev)
{
    BrCreateArgs mem_obj_rx = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .size = ALIGN_UP(8192 + 16, MEM_PAGE_SIZE),
        },
    };

    assert_br_success(br_create(&mem_obj_rx));

    BrMapArgs map_args = {
        .space = BR_SPACE_SELF,
        .mem_obj = mem_obj_rx.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_br_success(br_map(&map_args));
    dev->rx_buffer = (uint8_t *)map_args.vaddr;

    br_out32(dev->io_base + RTL8139_RBSTART_REG, mem_obj_rx.mem_obj.addr);
}

void rtl8139_init_interrupt(RTL8139Device *dev)
{
    br_out16(dev->io_base + RTL8139_IMR_REG, RTL8139_IMR_ROK | RTL8139_IMR_TOK);
    br_out32(dev->io_base + RTL8139_RCR_REG,
             RTL8139_RCR_ACCEPT_ALL | RTL8139_RCR_WRAP);
    br_out8(dev->io_base + RTL8139_CR_REG, RTL8139_CR_TE | RTL8139_CR_RE);

    BrCreateArgs rtl8139_irq = {
        .type = BR_OBJECT_IRQ,
        .irq = {
            .irq = 32 + 0xb, /* TODO: fetch from pci */
        },
    };

    assert_br_success(br_create(&rtl8139_irq));

    BrBindArgs rtl8139_bind = {
        .handle = rtl8139_irq.handle,
        .flags = BR_IRQ_NONE,
    };

    assert_br_success(br_bind(&rtl8139_bind));

    dev->irq_handle = rtl8139_irq.handle;
}

void rtl8139_handle_irq(MAYBE_UNUSED RTL8139Device *dev)
{
    uint16_t status = br_in16(dev->io_base + RTL8139_ISR_REG);

    if (status & RTL8139_ISR_ROK)
    {
        log$("Packet received - OK");
    } else if (status & RTL8139_ISR_RER)
    {
        log$("Packet receive error");
    } else if (status & RTL8139_ISR_TOK)
    {
        log$("Packet sent - ok");
    } else if (status & RTL8139_ISR_TER)
    {
        log$("Packet not sent - error");
    }

    br_out16(dev->io_base + RTL8139_ISR_REG, RTL8139_ISR_ROK | RTL8139_ISR_TOK);
}

RTL8139Device rtl8139_init()
{
    RTL8139Device dev = {.io_base = 0xc000}; /* TODO: fetch io base from pci */

    log$("Turning on the RTL8139");
    br_out8(dev.io_base + RTL8139_CONFIG1_REG, 0x0);

    log$("SoftReset the RTL8139");
    br_out8(dev.io_base + RTL8139_CR_REG, RTL8139_CONFIG1_LWACT);
    while ((br_in8(dev.io_base + RTL8139_CR_REG) & RTL8139_CONFIG1_LWACT) != 0)
    {
    }

    rtl8139_read_mac(&dev);

    log$("MAC: {02x}:{02x}:{02x}:{02x}:{02x}:{02x}", dev.mac[0], dev.mac[1],
         dev.mac[2], dev.mac[3], dev.mac[4], dev.mac[5]);

    rtl8139_init_rx(&dev);

    return dev;
}

/* Ethernet ----------------------------------------------------------------- */

/* ARP ---------------------------------------------------------------------- */
typedef struct PACKED
{
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_addr_len; // ethernet = 6
    uint8_t protocol_addr_len; // ipv4 = 4
    uint16_t opcode;
    uint8_t src_hardware_addr[6];
    uint8_t src_protocol_addr[4];
    uint8_t dest_hardware_addr[6];
    uint8_t dest_protocol_addr[4];
} Arp;

/* TCP/ip stack ------------------------------------------------------------- */

/* entry ---------------------------------------------------------------------*/

int main(void)
{
    log$("Hello from the net server!");
    RTL8139Device rtl8139_dev = rtl8139_init();

    BrIpcArgs ipc;
    ipc.flags = BR_IPC_RECV | BR_IPC_BLOCK;
    ipc.deadline = BR_DEADLINE_INFINITY;

    while (br_ipc(&ipc) == BR_SUCCESS)
    {
        BrMsg msg = ipc.msg;

        log$("Receive IPC from {}", ipc.msg.from);
        if (msg.from == BR_TASK_IRQ)
        {
            BrIrq irq = msg.arg[0];

            log$("IRQ: {}", irq);
            if (irq == (32 + 0xb))
            {
                br_ack(&(BrAckArgs){
                    .handle = rtl8139_dev.irq_handle,
                });

                rtl8139_handle_irq(&dev);
            }
        }
        else
        {
            log$("Unknown message!");
        }
    }
    return 0;
}
