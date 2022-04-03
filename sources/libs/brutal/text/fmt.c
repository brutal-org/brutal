#include <brutal/io/funcs.h>
#include <brutal/text/fmt.h>

Str str_fmt(Alloc *alloc, Str format, AnyVa args)
{
    Buf buf;
    buf_init(&buf, format.len, alloc);
    io_fmt(buf_writer(&buf), format, args);
    return buf_str(&buf);
}

Str str_printf(Alloc *alloc, Str format, ...)
{
    Buf buf;
    buf_init(&buf, format.len, alloc);

    va_list args;
    va_start(args, format);
    io_vprintf(buf_writer(&buf), format, args);
    va_end(args);

    return buf_str(&buf);
}
