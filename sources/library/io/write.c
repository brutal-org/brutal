#include <library/io/write.h>

struct write_result io_put(struct writer *writer, char c)
{
    return io_write(writer, &c, 1);
}

struct write_result print(struct writer *writer, str_t str)
{
    return io_write(writer, str.str, str.len);
}