#include <brutal-io>
#include <brutal-debug>
#include <bal/hw.h>
#include <bal/abi.h>
#include <brutal-alloc>
#include "bal/abi/types.h"
#include <brutal-net>
#include "driver.h"
#include "rtl8139.h"

static const uint8_t tsd_reg[4] = {RTL8139_TSD0,
                                    RTL8139_TSD1,
                                    RTL8139_TSD2,
                                    RTL8139_TSD3};
static const uint8_t tsad_reg[4] = {RTL8139_TSAD0,
                                    RTL8139_TSAD1,
                                    RTL8139_TSAD2,
                                    RTL8139_TSAD3};

static void rtl8139_read_mac(RTL8139Device *dev)
{
    uint32_t mac1 = bal_io_in32(dev->io, RTL8139_IDR0_REG);
    uint16_t mac2 = bal_io_in16(dev->io, RTL8139_IDR4_REG);
    dev->mac[0] = mac1 >> 0;
    dev->mac[1] = mac1 >> 8;
    dev->mac[2] = mac1 >> 16;
    dev->mac[3] = mac1 >> 24;
    dev->mac[4] = mac2 >> 0;
    dev->mac[5] = mac2 >> 8;
}

static uint8_t *rtl8139_get_mac(void *ctx)
{
    return ((RTL8139Device *)ctx)->mac;
}

static void rtl8139_init_rx(RTL8139Device *dev)
{
    // FIXME: umap and close
    BrCreateArgs mem_obj_rx = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .size = align_up$(8192 + 16 + 1500, MEM_PAGE_SIZE),
            .flags = BR_MEM_LOWER /* force lower memory */
        },
    };

    assert_br_success(br_create(&mem_obj_rx));

    BrMapArgs map_args = {
        .space = BR_HANDLE_SELF,
        .memory = mem_obj_rx.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_br_success(br_map(&map_args));
    dev->rx_buffer = (uint8_t *)map_args.vaddr;

    bal_io_out32(dev->io, RTL8139_RBSTART_REG, mem_obj_rx.memory.addr);
}

static void rtl8139_init_interrupt(RTL8139Device *dev)
{
    bal_io_out16(dev->io, RTL8139_IMR_REG, RTL8139_IMR_ROK | RTL8139_IMR_TOK | RTL8139_IMR_RER | RTL8139_IMR_TER);
    bal_io_out32(dev->io, RTL8139_RCR_REG,
             RTL8139_RCR_ACCEPT_ALL | RTL8139_RCR_WRAP);
    bal_io_out8(dev->io, RTL8139_CR_REG, RTL8139_CR_TE | RTL8139_CR_RE);
    log$("ligne 68");
    BrBindArgs rtl8139_bind = {
        .event = {
            .type = BR_EVENT_IRQ,
            .irq =  dev->int_line,
        }
    };
    br_bind(&rtl8139_bind);
    log$("interrupt initialized");
}

static void rtl8139_handle_irq(void *ctx)
{
    RTL8139Device *dev = (RTL8139Device *)ctx;

    uint16_t status = bal_io_in16(dev->io, RTL8139_ISR_REG);
    log$("status: {b}", status);
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

    EthernetFrameHeader *hdr = (EthernetFrameHeader*)dev->rx_buffer;

    log$("packet from: {02x}:{02x}:{02x}:{02x}:{02x}:{02x}", hdr->mac_dst[0], hdr->mac_dst[1],
         hdr->mac_dst[2], hdr->mac_dst[3], hdr->mac_dst[4], hdr->mac_dst[5]);
    bal_io_out16(dev->io, RTL8139_ISR_REG, RTL8139_ISR_ROK | RTL8139_ISR_TOK);
}

static void rtl8139_send(void *ctx, void *data, size_t len)
{
    RTL8139Device *dev = (RTL8139Device *)ctx;

    mem_cpy(((EthernetFrameHeader *)data)->mac_src, dev->mac, 6); /* copy source addr */

    BrCreateArgs mem_obj_tx = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .size = align_up$(8192 + 16 + 1500, MEM_PAGE_SIZE),
            .flags = BR_MEM_LOWER
        },
    };

    assert_br_success(br_create(&mem_obj_tx));

    BrMapArgs map_args = {
        .space = BR_HANDLE_SELF,
        .memory = mem_obj_tx.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_br_success(br_map(&map_args));
    uint8_t *data_copy = (uint8_t *)map_args.vaddr;
    mem_cpy(data_copy, data, len);

    log$("send packet (size: {})", len);
    bal_io_out32(dev->io, tsad_reg[dev->tx_curr], mem_obj_tx.memory.addr);
    bal_io_out32(dev->io, tsd_reg[dev->tx_curr], len);

    dev->tx_curr  = (dev->tx_curr + 1) % 4;

    assert_br_success(br_unmap(&(BrUnmapArgs){
        .space = BR_HANDLE_SELF,
        .vaddr = map_args.vaddr,
        .size = map_args.size,
    }));
    assert_br_success(br_close(&(BrCloseArgs){
        .handle = mem_obj_tx.handle
    }));
}

static void *rtl8139_init(PciConfigType0 *pci_conf, uint16_t int_line)
{
    RTL8139Device *dev;

    dev = alloc_malloc(alloc_global(), sizeof(RTL8139Device));
    dev->tx_curr = 0;

    dev->io = bal_io_port(pci_get_io_base(pci_conf), RTL8139_END_REG);
    dev->int_line = int_line;

    log$("Turning on the RTL8139");
    bal_io_out8(dev->io, RTL8139_CONFIG1_REG, 0x0);

    log$("SoftReset the RTL8139");
    bal_io_out8(dev->io, RTL8139_CR_REG, RTL8139_CONFIG1_LWACT);
    wait_for$((bal_io_in8(dev->io, RTL8139_CR_REG) & RTL8139_CONFIG1_LWACT) == 0);

    log$("trying to read mac");
    rtl8139_read_mac(dev);

    log$("MAC: {02x}:{02x}:{02x}:{02x}:{02x}:{02x}", dev->mac[0], dev->mac[1],
         dev->mac[2], dev->mac[3], dev->mac[4], dev->mac[5]);

    rtl8139_init_rx(dev);
    rtl8139_init_interrupt(dev);

    log$("RTL initialized");
    return dev;
}

static void rtl8139_deinit(void *ctx)
{
    RTL8139Device *dev = (RTL8139Device *)ctx;

    alloc_free(alloc_global(), dev);
}

NicDriver rtl8139_driver = {
    .init = rtl8139_init,
    .deinit = rtl8139_deinit,
    .send = rtl8139_send,
    .handle = rtl8139_handle_irq,
    .get_mac = rtl8139_get_mac
};
