#include <brutal/io/write.h>

IoWriteResult io_put(IoWriter *writer, char c)
{
    char buf[2] = {c, 0};
    return io_write(writer, buf, 1);
}

IoWriteResult io_print(IoWriter *writer, Str str)
{
    return io_write(writer, str.buffer, str.len);
}
