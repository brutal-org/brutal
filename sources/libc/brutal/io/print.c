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
    return (struct print_value){nullstr, PRINT_STRING, {._string = make_str(val)}};
}

struct print_value print_val_string(str_t val)
{
    return (struct print_value){nullstr, PRINT_STRING, {._string = val}};
}

struct print_value print_val_pointer(void *ptr)
{
    return (struct print_value){nullstr, PRINT_POINTER, {._pointer = ptr}};
}

write_result_t print_dispatch(struct writer *writer, struct fmt fmt, struct print_value value)
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
    }

    return OK(write_result_t, 0);
}

write_result_t print_impl(struct writer *writer, str_t format, struct print_args args)
{
    size_t current = 0;
    size_t written = 0;

    struct scan scan;
    scan_init(&scan, format);

    while (!scan_end(&scan))
    {
        if (scan_curr(&scan) == '{')
        {
            if (current < args.count)
            {
                auto fmt = fmt_parse(&scan);
                written += TRY(write_result_t, print_dispatch(writer, fmt, args.values[current]));
            }

            current++;
        }
        else
        {
            written += TRY(write_result_t, io_put(writer, scan_next(&scan)));
        }
    }

    return OK(write_result_t, written);
}
