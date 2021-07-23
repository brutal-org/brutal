#include <brutal/base/attributes.h>
#include <brutal/base/keywords.h>
#include <brutal/io/fmt.h>
#include <brutal/text/vals.h>
#include <ctype.h>
#include "brutal/io/write.h"

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

static void fmt_parse_attribute(struct fmt *fmt, Scan *scan)
{

    if (scan_skip_word(scan, str_cast("snake_case")))
    {
        fmt->flags |= FMT_FLAG_FORCE_SNAKE_CASE;
        return;
    }

    if (scan_skip_word(scan, str_cast("CamelCase")))
    {
        fmt->flags |= FMT_FLAG_FORCE_CAMEL_CASE;
        return;
    }

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

    case '#':
        fmt->flags |= FMT_FLAG_USE_PREFIX;
        break;
    case 'U':
        fmt->flags |= FMT_FLAG_FORCE_UPPER_CASE;
        break;
    case 'u':
        fmt->flags |= FMT_FLAG_FORCE_LOWER_CASE;
        break;
    }
    scan_next(scan);
}

static void fmt_parse_min_width(struct fmt *fmt, Scan *scan)
{
    if (scan_curr(scan) == '0')
    {
        fmt->flags |= FMT_FLAG_FILL_WITH_ZERO;
        scan_next(scan);
    }
    else
    {
        fmt->flags |= ~(FMT_FLAG_FILL_WITH_ZERO);
    }

    fmt->min_width = scan_next_decimal(scan);
}

struct fmt fmt_parse(Scan *scan)
{
    struct fmt fmt = {};
    fmt.flags = 0;

    scan_skip(scan, '{');

    while (scan_curr(scan) != '}' &&
           !scan_end(scan))
    {
        if (isdigit(scan_curr(scan)))
        {
            fmt_parse_min_width(&fmt, scan);
        }
        else
        {
            fmt_parse_attribute(&fmt, scan);
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
            if (self.flags & FMT_FLAG_FILL_WITH_ZERO)
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

    if (self.flags & FMT_FLAG_USE_PREFIX)
    {
        buffer[i++] = fmt_prefix(self);
        buffer[i++] = '0';
    }

    str_rvs(str_cast(buffer));

    return io_write(writer, buffer, i);
}

IoWriteResult fmt_char(MAYBE_UNUSED struct fmt self, IoWriter *writer, char character)
{
    size_t written = 0;

    written += TRY(IoWriteResult, io_put(writer, character));

    return OK(IoWriteResult, written);
}

IoWriteResult fmt_string_formatted(struct fmt self, IoWriter *writer, Str value)
{

    size_t written = 0;
    Scan scan;
    scan_init(&scan, value);

    while (!scan_end(&scan))
    {
        char out_char = scan_curr(&scan);

        if (self.flags & FMT_FLAG_FORCE_CAMEL_CASE)
        {

            // hello world => HelloWorld
            // hello_world => HelloWorld
            if (scan_skip(&scan, ' ') || scan_skip(&scan, '_'))
            {
                out_char = scan_curr(&scan);
                if (isalpha(out_char))
                {
                    out_char = toupper(out_char);
                }
            }
            else if (scan.head == 0)
            {
                // first char is always upper case
                if (isalpha(out_char))
                {
                    out_char = toupper(out_char);
                }
            }
        }
        else if (self.flags & FMT_FLAG_FORCE_SNAKE_CASE)
        {
            if (scan_skip(&scan, ' '))
            {

                written += TRY(IoWriteResult, io_put(writer, '_'));
                out_char = scan_curr(&scan);
            }

            if (isupper(out_char))
            {
                // don't try to put '_' if this is the first char:
                // HelloWorld must not result in: _hello_world
                if (scan.head != 0)
                    written += TRY(IoWriteResult, io_put(writer, '_'));

                out_char = tolower(out_char);
            }
        }

        if (self.flags & FMT_FLAG_FORCE_UPPER_CASE)
        {
            if (isalpha(out_char))
            {
                out_char = toupper(out_char);
            }
        }

        else if (self.flags & FMT_FLAG_FORCE_LOWER_CASE)
        {
            if (isalpha(out_char))
            {
                out_char = tolower(out_char);
            }
        }

        written += TRY(IoWriteResult, io_put(writer, (out_char)));
        scan_next(&scan);
    }
    return OK(IoWriteResult, written);
}

IoWriteResult fmt_string(MAYBE_UNUSED struct fmt self, IoWriter *writer, Str value)
{

    if (!(self.flags & FMT_STRING_MODIFICATION_FLAGS))
    {
        return io_print(writer, value);
    }

    return fmt_string_formatted(self, writer, value);
}
