#include <library/io/write.h>

write_r io_put(struct writer *writer, char c)
{
    return io_write(writer, &c, 1);
}

write_r print(struct writer *writer, str_t str)
{
    return io_write(writer, str.buffer, str.len);
}