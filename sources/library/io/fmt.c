#include <library/base/macros.h>
#include <library/io/fmt.h>
#include <library/text/vals.h>

static int fmt_base(struct fmt self)
{
    switch (self.type)
    {
    case FMT_BINARY:
        return 2;
    case FMT_OCTAL:
        return 8;
    default:
    case FMT_DECIMAL:
        return 10;
    case FMT_POINTER:
    case FMT_HEXADECIMAL:
        return 16;
    }
}

static char fmt_digit(struct fmt self, int d)
{
    return STR_UPPERCASE_XDIGITS[d % fmt_base(self)];
}

struct fmt fmt_parse(struct scan *scan)
{
    UNUSED(scan);

    struct fmt fmt = {};

    scan_skip(scan, '{');

    switch (scan_curr(scan))
    {
    case 's':
        fmt.type = FMT_STRING;
        break;

    case 'c':
        fmt.type = FMT_CHAR;
        break;

    case 'b':
        fmt.type = FMT_BINARY;
        break;

    case 'o':
        fmt.type = FMT_OCTAL;
        break;

    case 'd':
        fmt.type = FMT_DECIMAL;
        break;

    case 'p':
    case 'x':
        fmt.type = FMT_HEXADECIMAL;
        break;
    }

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
    if (value == 0)
    {
        return io_put(writer, '0');
    }

    size_t i = 0;
    char buffer[64] = {};

    while (value != 0)
    {
        buffer[i] = fmt_digit(self, value % fmt_base(self));
        value /= fmt_base(self);
        i++;
    }

    str_rvs(make_str(buffer));

    return io_write(writer, buffer, i);
}

write_r fmt_string(struct fmt self, struct writer *writer, str_t value)
{
    UNUSED(self);

    return print(writer, value);
}
