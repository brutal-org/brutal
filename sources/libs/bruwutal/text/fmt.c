#include <bruwutal/io/funcs.h>
#include <bruwutal/text/fmt.h>

Str str_fmt(Alloc *alloc, Str fmt, AnyVa args)
{
    Buf buf;
    buf_init(&buf, fmt.len, alloc);
    io_fmt(buf_writer(&buf), fmt, args);
    return buf_str(&buf);
}
