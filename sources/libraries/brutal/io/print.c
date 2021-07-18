#include <brutal/io/fmt.h>
#include <brutal/io/print.h>
#include <brutal/io/scan.h>

struct print_value print_val_signed(long val)
{
    return (struct print_value){nullstr, PRINT_SIGNED, {._signed = val}};
}

struct print_value print_val_unsigned(unsigned long val)
{
    return (struct print_value){nullstr, PRINT_UNSIGNED, {._unsigned = val}};
}

struct print_value print_val_cstring(char *val)
{
    return (struct print_value){nullstr, PRINT_STRING, {._string = str_cast(val)}};
}

struct print_value print_val_char(char val)
{
    return (struct print_value){nullstr, PRINT_CHAR, {._char = val}};
}

struct print_value print_val_string(Str val)
{
    return (struct print_value){nullstr, PRINT_STRING, {._string = val}};
}

struct print_value print_val_pointer(void *ptr)
{
    return (struct print_value){nullstr, PRINT_POINTER, {._pointer = ptr}};
}

IoWriteResult print_dispatch(IoWriter *writer, struct fmt fmt, struct print_value value)
{
    switch (value.type)
    {
    case PRINT_SIGNED:
        return fmt_signed(fmt, writer, value._signed);

    case PRINT_UNSIGNED:
        return fmt_unsigned(fmt, writer, value._unsigned);

    case PRINT_STRING:
        return fmt_string(fmt, writer, value._string);

    case PRINT_POINTER:
        return fmt_unsigned(fmt, writer, (uintptr_t)value._pointer);

    case PRINT_CHAR:
        return fmt_char(fmt, writer, value._char);
    }

    return OK(IoWriteResult, 0);
}

IoWriteResult print_impl(IoWriter *writer, Str format, struct print_args args)
{
    size_t current = 0;
    size_t written = 0;
    bool skip_fmt = false;
    struct scan scan;
    scan_init(&scan, format);

    while (!scan_end(&scan))
    {
        if (scan_curr(&scan) == '{' && !skip_fmt)
        {
            if (current < args.count)
            {
                auto fmt = fmt_parse(&scan);
                written += TRY(IoWriteResult, print_dispatch(writer, fmt, args.values[current]));
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
            written += TRY(IoWriteResult, io_put(writer, scan_next(&scan)));
        }
    }

    return OK(IoWriteResult, written);
}
