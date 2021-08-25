#include <brutal/base/attributes.h>
#include <brutal/base/cast.h>
#include <brutal/host/io.h>
#include <brutal/io/std.h>

static IoReadResult std_io_read(void *context, uint8_t *data, MAYBE_UNUSED size_t offset, size_t size)
{
    return host_io_read_std((IoStdChannel)context, data, size);
}

static IoWriteResult std_io_write(void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    return host_io_write_std((IoStdChannel)context, data, size);
}

static IoReader in = {
    .read = std_io_read,
    .context = (void *)IO_STD_IN,
};

static IoWriter out = {
    .write = std_io_write,
    .context = (void *)IO_STD_OUT,
};

static IoWriter err = {
    .write = std_io_write,
    .context = (void *)IO_STD_ERR,
};

IoReader *io_std_in(void)
{
    return &in;
}

IoWriter *io_std_out(void)
{
    return &out;
}

IoWriter *io_std_err(void)
{
    return &err;
}
