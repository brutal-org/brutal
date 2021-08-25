#include <brutal/base/attributes.h>
#include <brutal/base/cast.h>
#include <brutal/host/io.h>
#include <brutal/io/std.h>

static IoReadResult std_io_read(IoStdChannel *channel, uint8_t *data, MAYBE_UNUSED size_t offset, size_t size)
{
    return host_io_read_std(*channel, data, size);
}

static IoWriteResult std_io_write(IoStdChannel *channel, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    return host_io_write_std(*channel, data, size);
}

static IoStdChannel in_channel = IO_STD_IN;

static IoReader in_reader = {
    .read = (IoRead *)std_io_read,
    .context = (void *)&in_channel,
};

static IoStdChannel out_channel = IO_STD_OUT;

static IoWriter out_writer = {
    .write = (IoWrite *)std_io_write,
    .context = (void *)&out_channel,
};

static IoStdChannel err_channel = IO_STD_ERR;

static IoWriter err_writer = {
    .write = (IoWrite *)std_io_write,
    .context = (void *)&err_channel,
};

IoReader *io_std_in(void)
{
    return &in_reader;
}

IoWriter *io_std_out(void)
{
    return &out_writer;
}

IoWriter *io_std_err(void)
{
    return &err_writer;
}
