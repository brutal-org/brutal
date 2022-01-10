#include <brutal/base/attributes.h>
#include <brutal/debug/locked.h>
#include <brutal/io/fmt.h>
#include <brutal/io/print.h>
#include <brutal/parse/scan.h>

PrintValue print_val_signed(FmtInt val)
{
    return (PrintValue){nullstr, PRINT_SIGNED, {._signed = val}};
}

PrintValue print_val_unsigned(FmtUInt val)
{
    return (PrintValue){nullstr, PRINT_UNSIGNED, {._unsigned = val}};
}

PrintValue print_val_float(MAYBE_UNUSED double val)
{
#ifndef __freestanding__
    return (PrintValue){nullstr, PRINT_FLOAT, {._float = val}};
#else
    return (PrintValue){
        nullstr,
        PRINT_STRING,
        {._string = str$("<float>")},
    };
#endif
}

PrintValue print_val_cstring(char const *val)
{
    return (PrintValue){nullstr, PRINT_STRING, {._string = str$(val)}};
}

PrintValue print_val_char(char val)
{
    return (PrintValue){nullstr, PRINT_CHAR, {._char = val}};
}

PrintValue print_val_string(Str val)
{
    return (PrintValue){nullstr, PRINT_STRING, {._string = val}};
}

PrintValue print_val_pointer(void *ptr)
{
    return (PrintValue){nullstr, PRINT_POINTER, {._pointer = ptr}};
}

IoResult print_dispatch(IoWriter writer, Fmt fmt, PrintValue value)
{
    switch (value.type)
    {

    case PRINT_SIGNED:
        if (fmt.type == FMT_CHAR)
        {
            return fmt_char(fmt, writer, value._unsigned);
        }
        else
        {
            return fmt_signed(fmt, writer, value._signed);
        }

    case PRINT_UNSIGNED:
        if (fmt.type == FMT_CHAR)
        {
            return fmt_char(fmt, writer, value._unsigned);
        }
        else
        {
            return fmt_unsigned(fmt, writer, value._unsigned);
        }
#ifndef __freestanding__
    case PRINT_FLOAT:
        return fmt_float(fmt, writer, value._float);
#endif
    case PRINT_STRING:
        return fmt_string(fmt, writer, value._string);

    case PRINT_POINTER:
        return fmt_unsigned(fmt, writer, (uintptr_t)value._pointer);

    case PRINT_CHAR:
        return fmt_char(fmt, writer, value._char);
    default:
        panic$("No formater for value of type {}", value.type)
    }

    return OK(IoResult, 0);
}

IoResult print_impl(IoWriter writer, Str format, PrintArgs args)
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
            written += TRY(IoResult, io_putc(writer, '{'));
        }
        else if (scan_skip_word(&scan, str$("}}")))
        {
            skip_fmt = false;
            written += TRY(IoResult, io_putc(writer, '}'));
        }
        else if (scan_curr(&scan) == '{' && !skip_fmt)
        {
            Fmt fmt = fmt_parse(&scan);

            if (current < args.count)
            {
                written += TRY(IoResult, print_dispatch(writer, fmt, args.values[current]));
            }
            else
            {
                written += TRY(IoResult, io_print(writer, str$("{}")));
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
            written += TRY(IoResult, io_putc(writer, scan_next(&scan)));
        }
    }

    return OK(IoResult, written);
}

Str str_fmt_impl(Alloc *alloc, Str fmt, PrintArgs args)
{
    Buf buf;
    buf_init(&buf, fmt.len, alloc);
    print_impl(buf_writer(&buf), fmt, args);
    return buf_str(&buf);
}
