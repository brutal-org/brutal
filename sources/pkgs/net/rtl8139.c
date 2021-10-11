#include <brutal/io.h>
#include <brutal/log.h>
#include <syscalls/syscalls.h>
#include <syscalls/io.h>
#include <syscalls/helpers.h>
#include "rtl8139.h"

static const uint8_t tsd_reg[4] = {RTL8139_TSD0,
                                    RTL8139_TSD1,
                                    RTL8139_TSD2,
                                    RTL8139_TSD3};
static const uint8_t tsad_reg[4] = {RTL8139_TSAD0,
                                    RTL8139_TSAD1,
                                    RTL8139_TSAD2,
                                    RTL8139_TSAD3};
static RTL8139Device dev;

void rtl8139_read_mac()
{
    uint32_t mac1 = br_in32(dev.io_base + RTL8139_IDR0_REG);
    uint16_t mac2 = br_in16(dev.io_base + RTL8139_IDR4_REG);
    dev.mac[0] = mac1 >> 0;
    dev.mac[1] = mac1 >> 8;
    dev.mac[2] = mac1 >> 16;
    dev.mac[3] = mac1 >> 24;
    dev.mac[4] = mac2 >> 0;
    dev.mac[5] = mac2 >> 8;
}

static void rtl8139_init_rx()
{
    // FIXME: umap and close
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
    dev.rx_buffer = (uint8_t *)map_args.vaddr;

    br_out32(dev.io_base + RTL8139_RBSTART_REG, mem_obj_rx.mem_obj.addr);
}

static void rtl8139_init_interrupt()
{
    br_out16(dev.io_base + RTL8139_IMR_REG, RTL8139_IMR_ROK | RTL8139_IMR_TOK | RTL8139_IMR_RER | RTL8139_IMR_TER);
    br_out32(dev.io_base + RTL8139_RCR_REG,
             RTL8139_RCR_ACCEPT_ALL | RTL8139_RCR_WRAP);
    br_out8(dev.io_base + RTL8139_CR_REG, RTL8139_CR_TE | RTL8139_CR_RE);

    BrCreateArgs rtl8139_irq = {
        .type = BR_OBJECT_IRQ,
        .irq = {
            .irq = 0xb, /* TODO: fetch from pci */
        },
    };

    assert_br_success(br_create(&rtl8139_irq));

    BrBindArgs rtl8139_bind = {
        .handle = rtl8139_irq.handle,
        .flags = BR_IRQ_NONE,
    };

    assert_br_success(br_bind(&rtl8139_bind));

    dev.irq_handle = rtl8139_irq.handle;

    log$("interrupt initialized");
}

void rtl8139_handle_irq()
{
    uint16_t status = br_in16(dev.io_base + RTL8139_ISR_REG);
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

    br_out16(dev.io_base + RTL8139_ISR_REG, RTL8139_ISR_ROK | RTL8139_ISR_TOK);
}

void rtl8139_send_packet(uint8_t *data, size_t len)
{
    BrCreateArgs mem_obj_tx = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .size = ALIGN_UP(8192 + 16 + 1500, MEM_PAGE_SIZE),
        },
    };

    assert_br_success(br_create(&mem_obj_tx));

    BrMapArgs map_args = {
        .space = BR_SPACE_SELF,
        .mem_obj = mem_obj_tx.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_br_success(br_map(&map_args));
    uint8_t *data_copy = (uint8_t *)map_args.vaddr;
    mem_cpy(data_copy, data, len);

    log$("send packet (size: {})", len);
    br_out32(dev.io_base + tsad_reg[dev.tx_curr], mem_obj_tx.mem_obj.addr);
    br_out32(dev.io_base + tsd_reg[dev.tx_curr], len);

    dev.tx_curr  = (dev.tx_curr + 1) % 4;

    assert_br_success(br_unmap(&(BrUnmapArgs){
        .space = BR_SPACE_SELF,
        .vaddr = map_args.vaddr,
        .size = map_args.size,
    }));
    assert_br_success(brh_close(mem_obj_tx.handle));
}

RTL8139Device *rtl8139_init(PciConfigType0 *conf)
{
    dev.tx_curr = 0;

    dev.io_base = pci_get_base_io(conf);
    dev.int_line = conf->interrupt_line;

    log$("Turning on the RTL8139");
    br_out8(dev.io_base + RTL8139_CONFIG1_REG, 0x0);

    log$("SoftReset the RTL8139");
    br_out8(dev.io_base + RTL8139_CR_REG, RTL8139_CONFIG1_LWACT);
    WAIT_FOR((br_in8(dev.io_base + RTL8139_CR_REG) & RTL8139_CONFIG1_LWACT) == 0);

    log$("trying to read mac");
    rtl8139_read_mac();

    log$("MAC: {02x}:{02x}:{02x}:{02x}:{02x}:{02x}", dev.mac[0], dev.mac[1],
         dev.mac[2], dev.mac[3], dev.mac[4], dev.mac[5]);

    rtl8139_init_rx();
    rtl8139_init_interrupt();

    return &dev;
}
