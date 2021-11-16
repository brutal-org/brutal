#include <brutal/io.h>
#include <brutal/debug.h>
#include <bal/hw.h>
#include <bal/abi.h>
#include <brutal/alloc.h>
#include "bal/hw/io.h"
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

static void e1000_read_mac(E1000Device *dev)
{
    if (dev->has_eeprom)
    {
        log$("Reading from eeprom is unsupported for now");
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

static void e1000_handle_irq(void *ctx, uint16_t int_line)
{
    (void)ctx;
    (void)int_line;
}

static void e1000_send(void *ctx, void *data, size_t len)
{
    (void)ctx;
    (void)data;
    (void)len;
}

/* XXX: don't work */
static void *e1000_init(PciConfigType0 *pci_conf, uint16_t int_line)
{
    E1000Device *dev;

    (void)int_line;

    dev = alloc_malloc(alloc_global(), sizeof(E1000Device));

    /* 
    * TODO: crap and ugly code but you know in french we call that:
    * "flemme" and I think it's beautiful.
    */
    if (!(pci_conf->bars[0].address & 0x1))
    {
        dev->io = bal_io_mem(pci_conf->bars[0].address, 0 /* IDK lulz */);
    }
    else
    {
        dev->io = bal_io_port(pci_get_io_base(pci_conf), 0 /* IDK lulz */);
    }

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
