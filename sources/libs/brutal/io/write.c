#include <brutal/io/write.h>

IoResult io_write(IoWriter self, uint8_t const *data, size_t size)
{
    return self.write(self.context, data, size);
}

IoResult io_putc(IoWriter self, uint8_t c)
{
    return io_write(self, &c, 1);
}

IoResult io_print(IoWriter self, Str str)
{
    return io_write(self, (uint8_t *)str.buf, str.len);
}
