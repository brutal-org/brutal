#include <brutal/io/write.h>

IoWriteResult io_put(IoWriter *writer, char c)
{
    return io_write(writer, &c, 1);
}

IoWriteResult io_print(IoWriter *writer, Str str)
{
    return io_write(writer, str.buffer, str.len);
}
