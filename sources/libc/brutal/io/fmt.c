#include <brutal/base/attributes.h>
#include <brutal/base/keywords.h>
#include <brutal/io/fmt.h>
#include <brutal/text/vals.h>

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
    return STR_LOWERCASE_XDIGITS[d % fmt_base(self)];
}

static char fmt_prefix(struct fmt self)
{
    switch (self.type)
    {
    case FMT_BINARY:
        return 'b';
    case FMT_OCTAL:
        return 'o';
    default:
    case FMT_DECIMAL:
        return '0';
    case FMT_POINTER:
    case FMT_HEXADECIMAL:
        return 'x';
    }
}

static void fmt_parse_type(struct fmt *fmt, struct scan *scan)
{

    switch (scan_curr(scan))
    {
    case 's':
        fmt->type = FMT_STRING;
        break;

    case 'c':
        fmt->type = FMT_CHAR;
        break;

    case 'b':
        fmt->type = FMT_BINARY;
        break;

    case 'o':
        fmt->type = FMT_OCTAL;
        break;

    case 'd':
        fmt->type = FMT_DECIMAL;
        break;

    case 'p':
        fmt->type = FMT_POINTER;
        break;

    case 'x':
        fmt->type = FMT_HEXADECIMAL;
        break;
    }
}

static void fmt_parse_min_width(struct fmt *fmt, struct scan *scan)
{
    if (scan_curr(scan) == '0')
    {
        fmt->fill_with_zero = true;
        scan_next(scan);
    }
    else
    {
        fmt->fill_with_zero = false;
    }

    fmt->min_width = scan_next_decimal(scan);
}

struct fmt fmt_parse(struct scan *scan)
{
    struct fmt fmt = {};

    scan_skip(scan, '{');

    while (scan_curr(scan) != '}' &&
           !scan_end(scan))
    {
        if (scan_curr(scan) == '#')
        {
            scan_next(scan);
            fmt.prefix = true;
        }
        else if (scan_curr(scan) >= '0' && scan_curr(scan) <= '9')
        {
            fmt_parse_min_width(&fmt, scan);
        }
        else
        {
            fmt_parse_type(&fmt, scan);
            scan_next(scan);
        }
    }

    scan_skip(scan, '}');

    return fmt;
}

size_t fmt_write_length(struct fmt self, unsigned long value)
{
    size_t length = 0;
    for (; value != 0; length++)
    {
        value /= fmt_base(self);
    }
    return length;
}

IoWriteResult fmt_signed(struct fmt self, IoWriter *writer, long value)
{
    size_t written = 0;

    if (value < 0)
    {
        written += TRY(IoWriteResult, io_put(writer, '-'));
        value *= -1;
    }

    written += TRY(IoWriteResult, fmt_unsigned(self, writer, value));

    return OK(IoWriteResult, written);
}

IoWriteResult fmt_unsigned(struct fmt self, IoWriter *writer, unsigned long value)
{

    char buffer[64] = {};
    size_t i = 0;

    if (value == 0)
    {
        buffer[0] = '0';
        i++;
    }

    while (value != 0)
    {
        buffer[i] = fmt_digit(self, value % fmt_base(self));
        value /= fmt_base(self);
        i++;
    }

    if (self.min_width != 0)
    {

        while (i < self.min_width)
        {
            if (self.fill_with_zero)
            {
                buffer[i] = '0';
            }
            else
            {
                buffer[i] = ' ';
            }

            i++;
        }
    }

    if (self.prefix)
    {
        buffer[i++] = fmt_prefix(self);
        buffer[i++] = '0';
    }

    str_rvs(str_cast(buffer));

    return io_write(writer, buffer, i);
}

IoWriteResult fmt_string(MAYBE_UNUSED struct fmt self, IoWriter *writer, Str value)
{
    return io_print(writer, value);
}
