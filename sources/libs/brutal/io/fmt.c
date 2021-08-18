#include <brutal/alloc/global.h>
#include <brutal/base/attributes.h>
#include <brutal/base/keywords.h>
#include <brutal/io/fmt.h>
#include <brutal/text/case.h>
#include <brutal/text/utf8.h>
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

static void fmt_pase_case(struct fmt *fmt, Scan *scan)
{
    if (scan_skip_word(scan, str_cast("default")))
    {
        fmt->casing = CASE_DEFAULT;
    }
    else if (scan_skip_word(scan, str_cast("camel")))
    {
        fmt->casing = CASE_CAMEL;
    }
    else if (scan_skip_word(scan, str_cast("capital")))
    {
        fmt->casing = CASE_CAPITAL;
    }
    else if (scan_skip_word(scan, str_cast("constant")))
    {
        fmt->casing = CASE_CONSTANT;
    }
    else if (scan_skip_word(scan, str_cast("dot")))
    {
        fmt->casing = CASE_DOT;
    }
    else if (scan_skip_word(scan, str_cast("header")))
    {
        fmt->casing = CASE_HEADER;
    }
    else if (scan_skip_word(scan, str_cast("no")))
    {
        fmt->casing = CASE_NO;
    }
    else if (scan_skip_word(scan, str_cast("param")))
    {
        fmt->casing = CASE_PARAM;
    }
    else if (scan_skip_word(scan, str_cast("pascal")))
    {
        fmt->casing = CASE_PASCAL;
    }
    else if (scan_skip_word(scan, str_cast("path")))
    {
        fmt->casing = CASE_PATH;
    }
    else if (scan_skip_word(scan, str_cast("sentence")))
    {
        fmt->casing = CASE_SENTENCE;
    }
    else if (scan_skip_word(scan, str_cast("snake")))
    {
        fmt->casing = CASE_SNAKE;
    }
    else if (scan_skip_word(scan, str_cast("title")))
    {
        fmt->casing = CASE_TITLE;
    }
    else if (scan_skip_word(scan, str_cast("swap")))
    {
        fmt->casing = CASE_SWAP;
    }
    else if (scan_skip_word(scan, str_cast("lower")))
    {
        fmt->casing = CASE_LOWER;
    }
    else if (scan_skip_word(scan, str_cast("lower-first")))
    {
        fmt->casing = CASE_LOWER_FIRST;
    }
    else if (scan_skip_word(scan, str_cast("upper")))
    {
        fmt->casing = CASE_UPPER;
    }
    else if (scan_skip_word(scan, str_cast("upper-first")))
    {
        fmt->casing = CASE_UPPER_FIRST;
    }
    else if (scan_skip_word(scan, str_cast("sponge")))
    {
        fmt->casing = CASE_SPONGE;
    }
}

static void fmt_parse_type(struct fmt *fmt, Scan *scan)
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

static void fmt_parse_min_width(struct fmt *fmt, Scan *scan)
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

struct fmt fmt_parse(Scan *scan)
{
    struct fmt fmt = {};

    scan_skip(scan, '{');

    while (scan_curr(scan) != '}' &&
           !scan_end(scan))
    {
        scan_skip_space(scan);

        if (scan_curr(scan) == '#')
        {
            scan_next(scan);
            fmt.prefix = true;
        }
        else if (scan_curr(scan) >= '0' && scan_curr(scan) <= '9')
        {
            fmt_parse_min_width(&fmt, scan);
        }
        else if (scan_skip_word(scan, str_cast("case:")))
        {
            fmt_pase_case(&fmt, scan);
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

IoWriteResult fmt_char(MAYBE_UNUSED struct fmt self, IoWriter *writer, unsigned int character)
{
    size_t written = 0;

    auto utf8 = rune_to_utf8((Rune)character);

    written += TRY(IoWriteResult, io_print(writer, str_cast(&utf8)));

    return OK(IoWriteResult, written);
}

IoWriteResult fmt_string(struct fmt self, IoWriter *writer, Str value)
{
    if (self.casing == CASE_DEFAULT)
    {
        return io_print(writer, value);
    }
    else
    {
        Buffer buffer = case_change(self.casing, value, alloc_global());
        auto result = io_print(writer, buffer_str(&buffer));
        buffer_deinit(&buffer);
        return result;
    }
}
