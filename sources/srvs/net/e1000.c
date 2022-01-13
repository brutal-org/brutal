#include <brutal-io>
#include <brutal-debug>
#include <bal/hw.h>
#include <bal/abi.h>
#include <brutal-alloc>
#include "bal/hw/io.h"
#include <brutal-base>
#include <brutal-mem>
#include "driver.h"
#include "e1000.h"
#include "net/pci.h"

static void e1000_io_write(E1000Device *dev, uint16_t addr, uint32_t value)
{
    if (dev->io.type == BAL_IO_PORTS)
    {
        bal_io_out32(dev->io, 0, addr);
        bal_io_out32(dev->io, 4, value);
    }
    else
    {
        bal_io_out32(dev->io, addr, value);
    }
}

static uint32_t e1000_io_read(E1000Device *dev, uint16_t addr)
{
    if (dev->io.type == BAL_IO_PORTS)
    {
        bal_io_out32(dev->io, 0, addr);
        return bal_io_in32(dev->io, 4);
    }
    else
    {
        return bal_io_in32(dev->io, addr);
    }
}

static void e1000_detect_eeprom(E1000Device *dev)
{
    dev->has_eeprom = false;
    int i = 0;
    e1000_io_write(dev, E1000_EEPROM_REG, 0x1);
    do
    {
        uint32_t val = e1000_io_read(dev, E1000_EEPROM_REG);
        if (val & 0x10) /* XXX: magical value from osdev */
        {
            log$("Eeproom detected");
            dev->has_eeprom = true;
            break;
        }
        i++;
    }
    while (i < 1000);
}

static uint16_t e1000_eeprom_read(E1000Device *dev, uint8_t addr)
{
    uint32_t tmp = ((uint32_t)addr & 0xff) << 8;
    tmp |= 0x1;
    e1000_io_write(dev, E1000_EEPROM_REG, tmp);
    wait_for$(e1000_io_read(dev, E1000_EEPROM_REG) & 0x10);

    uint16_t data = e1000_io_read(dev, E1000_EEPROM_REG) >> 16;

    tmp = e1000_io_read(dev, E1000_EEPROM_REG);
    tmp &= ~0x1;
    e1000_io_write(dev, E1000_EEPROM_REG, tmp);
    return data;
}

static void e1000_read_mac(E1000Device *dev)
{
    if (dev->has_eeprom)
    {
        log$("Reading MAC addr from eeprom");
        uint16_t mac_part = e1000_eeprom_read(dev, 0x0);
        dev->mac[0] = mac_part >> 0;
        dev->mac[1] = mac_part >> 8;
        mac_part = e1000_eeprom_read(dev, 0x1);
        dev->mac[2] = mac_part >> 0;
        dev->mac[3] = mac_part >> 8;
        mac_part = e1000_eeprom_read(dev, 0x2);
        dev->mac[4] = mac_part >> 0;
        dev->mac[5] = mac_part >> 8;
    }
    else
    {
        uint32_t mac1 = bal_io_in32(dev->io, E1000_RAL_LOW_REG);
        uint16_t mac2 = bal_io_in16(dev->io, E1000_RAL_HIGH_REG);
        dev->mac[0] = mac1 >> 0;
        dev->mac[1] = mac1 >> 8;
        dev->mac[2] = mac1 >> 16;
        dev->mac[3] = mac1 >> 24;
        dev->mac[4] = mac2 >> 0;
        dev->mac[5] = mac2 >> 8;
    }
}

static uint8_t *e1000_get_mac(void *ctx)
{
    return ((E1000Device *)ctx)->mac;
}

static void e1000_handle_irq(void *ctx)
{
    E1000Device *dev = (E1000Device *)ctx;

    (void)dev;
}

static void e1000_send(void *ctx, void *data, size_t len)
{
    (void)ctx;
    (void)data;
    (void)len;
}

static void *e1000_init(PciConfigType0 *pci_conf, uint16_t int_line)
{
    E1000Device *dev;

    dev = alloc_malloc(alloc_global(), sizeof(E1000Device));

    /*
    * TODO: crap and ugly code but you know in french we call that:
    * "flemme" and I think it's beautiful.
    */
    if (!(pci_conf->bars[0] & 0x1))
    {
        bal_mem_init_pmm(&dev->mmio, pci_conf->bars[0], align_up$(E1000_END_REG, MEM_PAGE_SIZE));
        dev->io = bal_io_mem(&dev->mmio);
    }
    else
    {
        dev->io = bal_io_port(pci_get_io_base(pci_conf), E1000_END_REG);
    }

    dev->int_line = int_line;
    e1000_detect_eeprom(dev);

    log$("Trying to read mac");
    e1000_read_mac(dev);

    log$("MAC: {02x}:{02x}:{02x}:{02x}:{02x}:{02x}", dev->mac[0], dev->mac[1],
         dev->mac[2], dev->mac[3], dev->mac[4], dev->mac[5]);

    return dev;
}

static void e1000_deinit(void *ctx)
{
    E1000Device *dev = (E1000Device *)ctx;

    alloc_free(alloc_global(), dev);
}

NicDriver e1000_driver = {
    .init = e1000_init,
    .deinit = e1000_deinit,
    .send = e1000_send,
    .handle = e1000_handle_irq,
    .get_mac = e1000_get_mac
};
