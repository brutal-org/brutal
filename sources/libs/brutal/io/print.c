#include <brutal/io/fmt.h>
#include <brutal/io/print.h>
#include <brutal/parse/scan.h>

struct print_value print_val_signed(long val)
{
    return (struct print_value){nullstr, PRINT_SIGNED, {._signed = val}};
}

struct print_value print_val_unsigned(unsigned long long val)
{
    return (struct print_value){nullstr, PRINT_UNSIGNED, {._unsigned = val}};
}

struct print_value print_val_cstring(char const *val)
{
    return (struct print_value){nullstr, PRINT_STRING, {._string = str$(val)}};
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

struct print_value print_val_trans(PrintTrans trans)
{
    return (struct print_value){nullstr, PRINT_TRANS, {._trans = trans}};
}

IoWriteResult print_dispatch(IoWriter *writer, Fmt fmt, struct print_value value)
{
    switch (value.type)
    {

    case PRINT_TRANS:
        return print_impl(writer, value._trans.fmt, value._trans.args);
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

    case PRINT_STRING:
        return fmt_string(fmt, writer, value._string);

    case PRINT_POINTER:
        return fmt_unsigned(fmt, writer, (uintptr_t)value._pointer);

    case PRINT_CHAR:
        return fmt_char(fmt, writer, value._char);
    }

    return OK(IoWriteResult, 0);
}

IoWriteResult print_impl(IoWriter *writer, Str format, PrintArgs args)
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
            written += TRY(IoWriteResult, io_put(writer, '{'));
        }
        else if (scan_skip_word(&scan, str$("}}")))
        {
            skip_fmt = false;
            written += TRY(IoWriteResult, io_put(writer, '}'));
        }
        else if (scan_curr(&scan) == '{' && !skip_fmt)
        {
            Fmt fmt = fmt_parse(&scan);

            if (current < args.count)
            {
                written += TRY(IoWriteResult, print_dispatch(writer, fmt, args.values[current]));
            }
            else
            {
                written += TRY(IoWriteResult, io_print(writer, str$("{}")));
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
