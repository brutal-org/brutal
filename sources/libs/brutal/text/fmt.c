#include <brutal/io/funcs.h>
#include <brutal/text/fmt.h>

Str str_fmt(Alloc *alloc, Str fmt, AnyVa args)
{
    Buf buf;
    buf_init(&buf, fmt.len, alloc);
    io_fmt(buf_writer(&buf), fmt, args);
    return buf_str(&buf);
}
