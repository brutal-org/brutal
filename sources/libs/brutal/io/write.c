#include <brutal/io/write.h>

IoResult io_put(IoWriter *writer, uint8_t c)
{
    return io_write(writer, &c, 1);
}

IoResult io_print(IoWriter *writer, Str str)
{
    return io_write(writer, (uint8_t *)str.buffer, str.len);
}
