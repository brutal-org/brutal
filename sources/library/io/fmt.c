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
    return STR_LOWERCASE_XDIGITS[d % fmt_base(self)];
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
        fmt.type = FMT_POINTER;
        break;
    case 'x':
        fmt.type = FMT_HEXADECIMAL;
        break;
    }
    while (scan_curr(scan) != '}' &&
           !scan_end(scan))
    {
        if (scan_curr(scan) == 'a')
        {
            fmt.aligned = true;
        }
        scan_next(scan);
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

write_result_t fmt_signed(struct fmt self, struct writer *writer, long value)
{
    size_t written = 0;

    if (value < 0)
    {
        written += TRY(write_result_t, io_put(writer, '-'));
        value *= -1;
    }

    written += TRY(write_result_t, fmt_unsigned(self, writer, value));

    return OK(write_result_t, written);
}

write_result_t fmt_unsigned(struct fmt self, struct writer *writer, unsigned long value)
{

    char buffer[64] = {};
    size_t i = 0;
    while (value != 0)
    {
        buffer[i] = fmt_digit(self, value % fmt_base(self));
        value /= fmt_base(self);
        i++;
    }

    if (self.aligned)
    {
        size_t offset = 0;
        if (fmt_base(self) == 16)
        {
            offset = 16;
        }
        else
        {
            offset = 20;
        }

        while (i < offset)
        {
            buffer[i] = '0';
            i++;
        }
    }

    str_rvs(make_str(buffer));

    return io_write(writer, buffer, i);
}

write_result_t fmt_string(struct fmt self, struct writer *writer, str_t value)
{
    UNUSED(self);

    return print(writer, value);
}
