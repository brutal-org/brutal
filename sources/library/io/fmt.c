#include <library/base/macros.h>
#include <library/io/fmt.h>

struct fmt fmt_parse(struct scan *scan)
{
    UNUSED(scan);

    struct fmt fmt = {};

    scan_skip(scan, '{');

    while (scan_curr(scan) != '}' &&
           !scan_end(scan))
    {
        scan_next(scan);
    }

    scan_skip(scan, '}');

    return fmt;
}

write_r fmt_signed(struct fmt self, struct writer *writer, long value)
{
    size_t written = 0;

    if (value < 0)
    {
        written += TRY(io_put(writer, '-'));
        value *= -1;
    }

    written += TRY(fmt_unsigned(self, writer, value));

    return (write_r)OK(written);
}

write_r fmt_unsigned(struct fmt self, struct writer *writer, unsigned long value)
{
    UNUSED(self);
    UNUSED(writer);
    UNUSED(value);

    return (write_r)OK(0);
}

write_r fmt_string(struct fmt self, struct writer *writer, str_t value)
{
    UNUSED(self);

    return print(writer, value);
}
