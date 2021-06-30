#include <brutal/io/write.h>

WriteResult io_put(struct writer *writer, char c)
{
    return io_write(writer, &c, 1);
}

WriteResult print(struct writer *writer, Str str)
{
    return io_write(writer, str.buffer, str.len);
}
