#include <library/base/macros.h>
#include <library/io/fmt.h>
#include <library/io/print.h>
#include <library/io/scan.h>

struct print_value print_val_signed(long val)
{
    return (struct print_value){PRINT_SIGNED, {._signed = val}};
}

struct print_value print_val_unsigned(unsigned long val)
{
    return (struct print_value){PRINT_UNSIGNED, {._unsigned = val}};
}

struct print_value print_val_cstring(const char *val)
{
    return (struct print_value){PRINT_STRING, {._string = make_str(val)}};
}

struct print_value print_val_string(str_t val)
{
    return (struct print_value){PRINT_STRING, {._string = val}};
}

struct write_result print_dispatch(struct writer *writer, struct fmt fmt, struct print_value value)
{
    switch (value.type)
    {
    case PRINT_SIGNED:
        return fmt_signed(fmt, writer, value._signed);

    case PRINT_UNSIGNED:
        return fmt_unsigned(fmt, writer, value._unsigned);

    case PRINT_STRING:
        return fmt_string(fmt, writer, value._string);
    }

    return write_ok(0);
}

struct write_result print_details(struct writer *writer, str_t format, struct print_value *values, size_t count)
{
    size_t current = 0;
    size_t written = 0;

    struct scan scan;
    scan_init(&scan, format);

    while (scan_curr(&scan) != '\0' && !scan_end(&scan))
    {
        if (scan_curr(&scan) == '{')
        {
            if (current < count)
            {
                auto fmt = fmt_parse(&scan);
                auto result = print_dispatch(writer, fmt, values[current]);

                if (is_write_err(result))
                {
                    return result;
                }

                written += result.written;
            }

            current++;
        }
        else
        {
            auto result = io_put(writer, scan_next(&scan));

            if (is_write_err(result))
            {
                return result;
            }

            written += result.written;
        }
    }

    return write_ok(written);
}