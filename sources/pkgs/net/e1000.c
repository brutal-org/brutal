#include <brutal/io.h>
#include <brutal/debug.h>
#include <bal/hw.h>
#include <bal/abi.h>
#include <brutal/alloc.h>
#include "driver.h"
#include "e1000.h"

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

static void *e1000_init(uint32_t io_base, uint16_t int_line)
{
    E1000Device *dev;

    log$("hello e1000");
    (void)int_line;
    dev = alloc_malloc(alloc_global(), sizeof(E1000Device));

    dev->io = bal_io_port(io_base, 0 /* IDK lulz */);

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
