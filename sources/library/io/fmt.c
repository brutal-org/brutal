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

struct write_result fmt_signed(struct fmt self, struct writer *writer, long value)
{
    UNUSED(self);
    UNUSED(writer);
    UNUSED(value);

    return write_ok(0);
}

struct write_result fmt_unsigned(struct fmt self, struct writer *writer, unsigned long value)
{
    UNUSED(self);
    UNUSED(writer);
    UNUSED(value);

    return write_ok(0);
}

struct write_result fmt_string(struct fmt self, struct writer *writer, str_t value)
{
    UNUSED(self);

    return print(writer, value);
}
