#include <embed/chan.h>
#include <embed/file.h>

#include "chan.h"

static IoResult io_chan_read(void *ctx, uint8_t *data, size_t size)
{
    return embed_chan_read(*(IoChan *)ctx, data, size);
}

static IoResult io_chan_write(void *ctx, uint8_t const *data, size_t size)
{
    return embed_chan_write(*(IoChan *)ctx, data, size);
}

IoReader io_chan_in(void)
{
    static IoChan channel = IO_CHAN_IN;
    return (IoReader){
        .read = io_chan_read,
        .context = (void *)&channel,
    };
}

IoWriter io_chan_out(void)
{
    static IoChan channel = IO_CHAN_OUT;
    return (IoWriter){
        .write = io_chan_write,
        .context = (void *)&channel,
    };
}

IoWriter io_chan_err(void)
{
    static IoChan channel = IO_CHAN_ERR;
    return (IoWriter){
        .write = io_chan_write,
        .context = (void *)&channel,
    };
}
