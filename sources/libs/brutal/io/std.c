#include <brutal/base/cast.h>
#include <brutal/host/io.h>
#include <brutal/io/std.h>

static IoReadResult std_io_read(IoReader *reader, char *data, size_t size)
{
    return host_io_read_std(((IoStdReader *)reader)->channel, data, size);
}

static IoWriteResult std_io_write(IoWriter *writer, char const *data, size_t size)
{
    return host_io_write_std(((IoStdWriter *)writer)->channel, data, size);
}

static IoStdReader in = {
    .base = {
        .read = std_io_read,
    },
    .channel = IO_STD_IN,
};

static IoStdWriter out = {
    .base = {
        .write = std_io_write,
    },
    .channel = IO_STD_OUT,
};

static IoStdWriter err = {
    .base = {
        .write = std_io_write,
    },
    .channel = IO_STD_ERR,
};

IoReader *io_std_in(void)
{
    return base_cast(&in);
}

IoWriter *io_std_out(void)
{
    return base_cast(&out);
}

IoWriter *io_std_err(void)
{
    return base_cast(&err);
}
