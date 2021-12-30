#include <brutal/base/attributes.h>
#include <brutal/io/std.h>
#include <embed/chan.h>
#include <embed/file.h>

static IoResult io_chan_read(IoChan *channel, uint8_t *data, MAYBE_UNUSED size_t offset, size_t size)
{
    return embed_chan_read(*channel, data, size);
}

static IoResult io_chan_write(IoChan *channel, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    return embed_chan_write(*channel, data, size);
}

static IoChan in_channel = IO_CHAN_IN;

static IoReader in_reader = {
    .read = (IoRead *)io_chan_read,
    .context = (void *)&in_channel,
};

static IoChan out_channel = IO_CHAN_OUT;

static IoWriter out_writer = {
    .write = (IoWrite *)io_chan_write,
    .context = (void *)&out_channel,
};

static IoChan err_channel = IO_CHAN_ERR;

static IoWriter err_writer = {
    .write = (IoWrite *)io_chan_write,
    .context = (void *)&err_channel,
};

IoReader *io_chan_in(void)
{
    return &in_reader;
}

IoWriter *io_chan_out(void)
{
    return &out_writer;
}

IoWriter *io_chan_err(void)
{
    return &err_writer;
}
