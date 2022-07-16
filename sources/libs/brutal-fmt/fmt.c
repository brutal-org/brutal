#define _BRUTAL_SOURCE

#include <brutal-alloc>
#include <brutal-parse>
#include <brutal-text>
#include <math.h>

#include "fmt.h"
#include "funcs.h"

static int fmt_base(Fmt self)
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

static char fmt_digit(Fmt self, int d)
{
    return STR_LOWERCASE_XDIGITS[d % fmt_base(self)];
}

static char fmt_prefix(Fmt self)
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

static FmtType fmt_parse_type(char c)
{
    switch (c)
    {
    case 's':
        return FMT_STRING;

    case 'c':
        return FMT_CHAR;

    case 'b':
        return FMT_BINARY;

    case 'o':
        return FMT_OCTAL;

    case 'd':
    case 'i':
    case 'u':
        return FMT_DECIMAL;

    case 'p':
        return FMT_POINTER;

    case 'X':
    case 'x':
        return FMT_HEXADECIMAL;

    default:
        return FMT_NONE;
    }
}

static bool fmt_parse_precision(Fmt *fmt, Scan *scan)
{
    if (scan_skip(scan, '0'))
    {
        fmt->fill = '0';
    }

    bool any = false;

    if (scan_next_int(scan, &fmt->width))
    {
        any = true;
    }

    if (!scan_skip(scan, '.'))
    {
        return any;
    }

    if (!scan_next_int(scan, &fmt->precision))
    {
        fmt->precision = 0;
    }

    return true;
}

Fmt fmt_parse(Scan *scan)
{
    Fmt fmt = {};
    fmt.precision = FMT_PRECISION_DEFAULT;
    fmt.width = 0;
    fmt.fill = ' ';

    scan_skip(scan, '{');

    while (scan_curr(scan) != '}' &&
           !scan_ended(scan))
    {
        scan_skip_space(scan);

        if (scan_skip(scan, '#'))
        {
            fmt.prefix = true;
        }
        else if (scan_skip(scan, '+'))
        {
            fmt.prefix_plus = true;
        }
        else if (scan_skip_word(scan, str$("case-")))
        {
            fmt.casing = case_parse(scan);
        }
        else if (!fmt_parse_precision(&fmt, scan))
        {
            fmt.type = fmt_parse_type(scan_next(scan));
        }
    }

    scan_skip(scan, '}');

    return fmt;
}

FmtPrintfType fmt_parse_printf_type(Scan *scan, Fmt *fmt)
{
    if (scan_skip_word(scan, str$("ll")))
    {
        char c = scan_next(scan);
        fmt->type = fmt_parse_type(c);

        if (c == 'd' || c == 'i')
        {
            return FMT_PRINTF_LONGLONG;
        }
        else
        {
            return FMT_PRINTF_ULONGLONG;
        }
    }
    else if (scan_skip(scan, 'l'))
    {
        char c = scan_next(scan);
        fmt->type = fmt_parse_type(c);

        if (c == 'd' || c == 'i')
        {
            return FMT_PRINTF_LONG;
        }
        else
        {
            return FMT_PRINTF_ULONG;
        }
    }
    else if (scan_skip(scan, 'p'))
    {
        return FMT_PRINTF_PTR;
    }
    else if (scan_skip(scan, 's'))
    {
        return FMT_PRINTF_STRING;
    }
    else
    {
        char c = scan_next(scan);
        fmt->type = fmt_parse_type(c);

        if (c == 'd' || c == 'i')
        {
            return FMT_PRINTF_INT;
        }
        else
        {
            return FMT_PRINTF_UINT;
        }
    }
}

FmtPrintfType fmt_parse_printf(Scan *scan, Fmt *fmt)
{
    *fmt = (Fmt){};
    fmt->precision = FMT_PRECISION_DEFAULT;
    fmt->width = 1;
    fmt->fill = ' ';

    scan_skip(scan, '%');

    while (!scan_ended(scan))
    {
        if (scan_skip(scan, '-'))
        {
        }
        else if (scan_skip(scan, '+'))
        {
            fmt->prefix_plus = true;
        }
        else if (scan_skip(scan, '#'))
        {
            fmt->prefix = true;
        }
        else if (!fmt_parse_precision(fmt, scan))
        {
            return fmt_parse_printf_type(scan, fmt);
        }
    }

    return FMT_PRINTF_INT;
}

static void reverse(uint8_t *str, size_t len)
{
    for (size_t i = 0; i < len / 2; i++)
    {
        swap$(&str[i], &str[len - i - 1]);
    }
}

IoResult fmt_signed(Fmt self, IoWriter writer, int64_t value)
{
    int i = 0;
    uint8_t buf[128 * 8] = {};
    bool negative = false;

    if (value < 0)
    {
        negative = true;
        value = -value;
    }

    if (self.precision == FMT_PRECISION_DEFAULT)
    {
        self.precision = 1;
    }
    else
    {
        self.fill = ' ';
    }

    while (value != 0)
    {
        buf[i++] = fmt_digit(self, value % fmt_base(self));
        value /= fmt_base(self);
    }

    while (i < self.precision)
    {
        buf[i++] = '0';
    }

    if (self.prefix)
    {
        buf[i++] = fmt_prefix(self);
        buf[i++] = '0';
    }

    if (negative)
    {
        buf[i++] = '-';
    }
    else if (self.prefix_plus)
    {
        buf[i++] = '+';
    }

    while (i < self.width)
    {
        buf[i++] = self.fill;
    }

    reverse(buf, i);

    return io_write(writer, buf, i);
}

IoResult fmt_unsigned(Fmt self, IoWriter writer, uint64_t value)
{
    int i = 0;
    uint8_t buf[128 * 8] = {};

    if (self.precision == FMT_PRECISION_DEFAULT)
    {
        self.precision = 1;
    }
    else
    {
        self.fill = ' ';
    }

    while (value != 0)
    {
        buf[i++] = fmt_digit(self, value % fmt_base(self));
        value /= fmt_base(self);
    }

    while (i < self.precision)
    {
        buf[i++] = '0';
    }

    if (self.prefix)
    {
        buf[i++] = fmt_prefix(self);
        buf[i++] = '0';
    }

    if (self.prefix_plus)
    {
        buf[i++] = '+';
    }

    while (i < self.width)
    {
        buf[i++] = self.fill;
    }

    reverse(buf, i);

    return io_write(writer, buf, i);
}

#ifndef __freestanding__

IoResult fmt_float(Fmt self, IoWriter writer, double value)
{
    if (self.precision == FMT_PRECISION_DEFAULT)
    {
        self.precision = 6;
    }

    if (isnan(value))
    {
        return io_write_str(writer, str$("nan"));
    }

    int written = 0;

    if (value < 0)
    {
        written += TRY(IoResult, io_write_byte(writer, '-'));
        value *= -1;
    }

    if (isinf(value))
    {
        written += TRY(IoResult, io_write_str(writer, str$("inf")));
        return OK(IoResult, written);
    }

    written += TRY(IoResult, fmt_unsigned(self, writer, (uint64_t)value));

    if (self.precision == 0)
    {
        return OK(IoResult, written);
    }

    written += TRY(IoResult, io_write_byte(writer, '.'));

    value -= (uint64_t)value;

    for (int i = 0; i < self.precision; i++)
    {
        value *= fmt_base(self);
        written += TRY(IoResult, io_write_byte(writer, fmt_digit(self, (uint64_t)value)));
        value -= (uint64_t)value;
    }

    return OK(IoResult, written);
}

#endif

IoResult fmt_char(Fmt self, IoWriter writer, unsigned int character)
{
    size_t written = 0;

    switch (self.casing)
    {
    case CASE_CAPITAL:
    case CASE_CONSTANT:
    case CASE_UPPER_FIRST:
    case CASE_UPPER:
        character = toupper(character);
        break;

    case CASE_NO:
    case CASE_CAMEL:
    case CASE_LOWER_FIRST:
    case CASE_LOWER:
        character = tolower(character);
        break;

    case CASE_SWAP:
        if (isupper(character))
        {
            character = tolower(character);
        }
        else
        {
            character = toupper(character);
        }
        break;

    default:
        break;
    }

    StrFix8 utf8 = utf8_from_rune((Rune)character);

    written += TRY(IoResult, io_write_str(writer, str$(&utf8)));

    return OK(IoResult, written);
}

IoResult fmt_string(Fmt self, IoWriter writer, Str value)
{
    if (self.casing == CASE_DEFAULT)
    {
        return io_write_str(writer, value);
    }
    else
    {
        Buf buf = case_change(self.casing, value, alloc_global());
        IoResult result = io_write_str(writer, buf_str(&buf));
        buf_deinit(&buf);
        return result;
    }
}

IoResult fmt_any(Fmt self, IoWriter writer, Any value)
{
    size_t res = 0;

    switch (value.type)
    {
    case ANY_INT:
    {
        if (self.type == FMT_CHAR)
        {
            res += TRY(IoResult, fmt_char(self, writer, value.int_));
            break;
        }
        else
        {
            res += TRY(IoResult, fmt_signed(self, writer, value.int_));
            break;
        }
    }

    case ANY_UINT:
    {
        if (self.type == FMT_CHAR)
        {
            res += TRY(IoResult, fmt_char(self, writer, value.uint_));
            break;
        }
        else
        {
            res += TRY(IoResult, fmt_unsigned(self, writer, value.uint_));
            break;
        }
    }

#ifndef __freestanding__
    case ANY_FLOAT:
    {
        res += TRY(IoResult, fmt_float(self, writer, value.float_));
        break;
    }
#endif

    case ANY_STR:
    {
        res += TRY(IoResult, fmt_string(self, writer, value.str_));
        break;
    }

    case ANY_PTR:
    {
        res += TRY(IoResult, fmt_unsigned(self, writer, (uintptr_t)value.ptr_));
        break;
    }

    default:
    {
        res += TRY(IoResult, io_write_str(writer, str$("<unknown>")));
        break;
    }
    }

    return OK(IoResult, res);
}
