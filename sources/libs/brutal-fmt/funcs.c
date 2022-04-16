#include "funcs.h"

IoResult fmt_format(IoWriter writer, Str format, AnyVa args)
{
    size_t current = 0;
    size_t written = 0;
    bool skip_fmt = false;
    Scan scan;
    scan_init(&scan, format);

    while (!scan_ended(&scan))
    {
        if (scan_skip_word(&scan, str$("{{")))
        {
            skip_fmt = false;
            written += TRY(IoResult, io_write_byte(writer, '{'));
        }
        else if (scan_skip_word(&scan, str$("}}")))
        {
            skip_fmt = false;
            written += TRY(IoResult, io_write_byte(writer, '}'));
        }
        else if (scan_curr(&scan) == '{' && !skip_fmt)
        {
            Fmt fmt = fmt_parse(&scan);

            if (current < args.len)
            {
                written += TRY(IoResult, fmt_any(fmt, writer, args.buf[current]));
            }
            else
            {
                written += TRY(IoResult, io_write_str(writer, str$("{}")));
            }

            current++;
        }
        else if (scan_curr(&scan) == '\\' && skip_fmt == false)
        {
            skip_fmt = true;
            scan_next(&scan);
        }
        else
        {
            skip_fmt = false;
            written += TRY(IoResult, io_write_byte(writer, scan_next(&scan)));
        }
    }

    return OK(IoResult, written);
}

IoResult fmt_vprintf(IoWriter writer, Str format, va_list va)
{
    Scan scan;
    size_t written = 0;
    scan_init(&scan, format);

    while (!scan_ended(&scan))
    {
        if (scan_skip_word(&scan, str$("%%")))
        {
            written += TRY(IoResult, io_write_byte(writer, '%'));
        }
        else if (scan_curr(&scan) == '%')
        {
            Fmt fmt = {};
            FmtPrintfType type = fmt_parse_printf(&scan, &fmt);

            switch (type)
            {
            default:
            case FMT_PRINTF_INT:
                written += TRY(IoResult, fmt_signed(fmt, writer, va_arg(va, int)));
                break;

            case FMT_PRINTF_LONG:
                written += TRY(IoResult, fmt_signed(fmt, writer, va_arg(va, long)));
                break;

            case FMT_PRINTF_LONGLONG:
                written += TRY(IoResult, fmt_signed(fmt, writer, va_arg(va, long long)));
                break;

            case FMT_PRINTF_UINT:
                written += TRY(IoResult, fmt_unsigned(fmt, writer, va_arg(va, unsigned int)));
                break;

            case FMT_PRINTF_ULONG:
                written += TRY(IoResult, fmt_unsigned(fmt, writer, va_arg(va, unsigned long)));
                break;

            case FMT_PRINTF_ULONGLONG:
                written += TRY(IoResult, fmt_unsigned(fmt, writer, va_arg(va, unsigned long long)));
                break;

#ifndef __freestanding__
            case FMT_PRINTF_FLOAT:
                written += TRY(IoResult, fmt_float(fmt, writer, va_arg(va, double)));
                break;
#endif

            case FMT_PRINTF_STRING:
                written += TRY(IoResult, fmt_string(fmt, writer, str$(va_arg(va, char const *))));
                break;

            case FMT_PRINTF_PTR:
                written += TRY(IoResult, fmt_unsigned(fmt, writer, va_arg(va, uintptr_t)));
                break;
            }
        }
        else
        {
            written += TRY(IoResult, io_write_byte(writer, scan_next(&scan)));
        }
    }

    return OK(IoResult, written);
}

IoResult fmt_printf(IoWriter writer, Str format, ...)
{
    va_list va;
    va_start(va, format);
    IoResult result = fmt_vprintf(writer, format, va);
    va_end(va);
    return result;
}

Str fmt_str(Alloc *alloc, Str format, AnyVa args)
{
    Buf buf;
    buf_init(&buf, format.len, alloc);
    fmt_format(buf_writer(&buf), format, args);
    return buf_str(&buf);
}

Str fmt_sprintf(Alloc *alloc, Str format, ...)
{
    Buf buf;
    buf_init(&buf, format.len, alloc);

    va_list args;
    va_start(args, format);
    fmt_vprintf(buf_writer(&buf), format, args);
    va_end(args);

    return buf_str(&buf);
}
