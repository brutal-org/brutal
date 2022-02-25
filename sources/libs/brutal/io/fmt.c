#define _BRUTAL_SOURCE

#include <brutal/alloc/global.h>
#include <brutal/base/attributes.h>
#include <brutal/base/keywords.h>
#include <brutal/io/fmt.h>
#include <brutal/io/funcs.h>
#include <brutal/parse/nums.h>
#include <brutal/text/case.h>
#include <brutal/text/utf8.h>
#include <brutal/text/vals.h>
#include <math.h>

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

const char *color_table[] = {
    [FMT_COL_NONE] = "none",
    [FMT_BLACK] = "black",
    [FMT_RED] = "red",
    [FMT_WHITE] = "white",
    [FMT_GREEN] = "green",
    [FMT_BLUE] = "blue",
    [FMT_YELLOW] = "yellow",
    [FMT_MAGENTA] = "magenta",
    [FMT_CYAN] = "cyan",
    [FMT_GRAY] = "gray",
    [FMT_BLACK_GRAY] = "dark-gray",
};

static FmtColor fmt_parse_color(Scan *scan)
{
    FmtColor col = {};
    if (scan_skip_word(scan, str$("light-")))
    {
        col.bright = true;
    }

    for (size_t i = 0; i < sizeof(color_table) / sizeof(color_table[0]); i++)
    {
        if (scan_skip_word(scan, str$(color_table[i])))
        {
            col.type = (FmtColorTypes)i;
            break;
        }
    }

    return col;
}

static bool fmt_parse_style(Fmt *fmt, Scan *scan)
{
    if (scan_skip_word(scan, str$("underline")))
    {
        fmt->style.underline = true;
        return true;
    }
    else if (scan_skip_word(scan, str$("bold")))
    {
        fmt->style.bold = true;
        return true;
    }
    else if (scan_skip_word(scan, str$("bg-")))
    {
        fmt->style.bg_color = fmt_parse_color(scan);
        return true;
    }
    else if (scan_skip_word(scan, str$("fg-")))
    {
        fmt->style.fg_color = fmt_parse_color(scan);
        return true;
    }
    return false;
}

static void fmt_parse_case(Fmt *fmt, Scan *scan)
{
    if (scan_skip_word(scan, str$("default")))
    {
        fmt->casing = CASE_DEFAULT;
    }
    else if (scan_skip_word(scan, str$("camel")))
    {
        fmt->casing = CASE_CAMEL;
    }
    else if (scan_skip_word(scan, str$("capital")))
    {
        fmt->casing = CASE_CAPITAL;
    }
    else if (scan_skip_word(scan, str$("constant")))
    {
        fmt->casing = CASE_CONSTANT;
    }
    else if (scan_skip_word(scan, str$("dot")))
    {
        fmt->casing = CASE_DOT;
    }
    else if (scan_skip_word(scan, str$("header")))
    {
        fmt->casing = CASE_HEADER;
    }
    else if (scan_skip_word(scan, str$("no")))
    {
        fmt->casing = CASE_NO;
    }
    else if (scan_skip_word(scan, str$("param")))
    {
        fmt->casing = CASE_PARAM;
    }
    else if (scan_skip_word(scan, str$("pascal")))
    {
        fmt->casing = CASE_PASCAL;
    }
    else if (scan_skip_word(scan, str$("path")))
    {
        fmt->casing = CASE_PATH;
    }
    else if (scan_skip_word(scan, str$("sentence")))
    {
        fmt->casing = CASE_SENTENCE;
    }
    else if (scan_skip_word(scan, str$("snake")))
    {
        fmt->casing = CASE_SNAKE;
    }
    else if (scan_skip_word(scan, str$("title")))
    {
        fmt->casing = CASE_TITLE;
    }
    else if (scan_skip_word(scan, str$("swap")))
    {
        fmt->casing = CASE_SWAP;
    }
    else if (scan_skip_word(scan, str$("lower")))
    {
        fmt->casing = CASE_LOWER;
    }
    else if (scan_skip_word(scan, str$("lower-first")))
    {
        fmt->casing = CASE_LOWER_FIRST;
    }
    else if (scan_skip_word(scan, str$("upper")))
    {
        fmt->casing = CASE_UPPER;
    }
    else if (scan_skip_word(scan, str$("upper-first")))
    {
        fmt->casing = CASE_UPPER_FIRST;
    }
    else if (scan_skip_word(scan, str$("sponge")))
    {
        fmt->casing = CASE_SPONGE;
    }
}

static void fmt_parse_type(Fmt *fmt, Scan *scan)
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

static void fmt_parse_min_width(Fmt *fmt, Scan *scan)
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

    scan_next_int(scan, &fmt->min_width);
}

Fmt fmt_parse(Scan *scan)
{
    Fmt fmt = {};
    fmt.precison = 6;

    scan_skip(scan, '{');

    while (scan_curr(scan) != '}' &&
           !scan_ended(scan))
    {
        scan_skip_space(scan);

        if (scan_curr(scan) == '#')
        {
            scan_next(scan);
            fmt.prefix = true;
        }
        else if (fmt_parse_style(&fmt, scan))
        {
            fmt.style.has_style = true;
        }
        else if (scan_curr(scan) >= '0' && scan_curr(scan) <= '9')
        {
            fmt_parse_min_width(&fmt, scan);
        }
        else if (scan_skip_word(scan, str$("case:")))
        {
            fmt_parse_case(&fmt, scan);
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

IoResult fmt_signed(Fmt self, IoWriter writer, int64_t value)
{
    size_t written = 0;

    if (value < 0)
    {
        written += TRY(IoResult, io_write_byte(writer, '-'));
        value *= -1;
    }

    written += TRY(IoResult, fmt_unsigned(self, writer, value));

    return OK(IoResult, written);
}

static void reverse(uint8_t *str, size_t len)
{
    for (uint8_t *p1 = str, *p2 = str + len - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
}

IoResult fmt_unsigned(Fmt self, IoWriter writer, uint64_t value)
{
    uint8_t buf[sizeof(uint64_t) * 8] = {};
    int i = 0;

    if (value == 0)
    {
        buf[0] = '0';
        i++;
    }

    while (value != 0)
    {
        buf[i] = fmt_digit(self, value % fmt_base(self));
        value /= fmt_base(self);
        i++;
    }

    if (self.min_width != 0)
    {
        while (i < self.min_width)
        {
            if (self.fill_with_zero)
            {
                buf[i] = '0';
            }
            else
            {
                buf[i] = ' ';
            }

            i++;
        }
    }

    if (self.prefix)
    {
        buf[i++] = fmt_prefix(self);
        buf[i++] = '0';
    }

    reverse(buf, i);

    return io_write(writer, buf, i);
}

#ifndef __freestanding__

IoResult fmt_float(Fmt self, IoWriter writer, double value)
{
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

    if (self.precison == 0)
    {
        return OK(IoResult, written);
    }

    written += TRY(IoResult, io_write_byte(writer, '.'));

    value -= (uint64_t)value;

    for (int i = 0; i < self.precison; i++)
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

// FIXME: maybe put it in a specific header later.
#define ANSI_ESC_BRIGHT 60
#define ANSI_ESC_FG 30
#define ANSI_ESC_BG 40

static int fmt_color_id(FmtColor color)
{
    int offset = 0;

    if(color.bright)
    {
        offset += ANSI_ESC_BRIGHT;
    }

    switch (color.type)
    {
    case FMT_BLACK:
        return  0 + offset;
    case FMT_RED:
        return 1 + offset;
    case FMT_GREEN:
        return 2 + offset;
    case FMT_YELLOW:
        return 3 + offset;
    case FMT_BLUE:
        return 4 + offset;
    case FMT_MAGENTA:
        return 5 + offset;
    case FMT_CYAN:
        return 6 + offset;
    case FMT_WHITE:
        return 7 + ANSI_ESC_BRIGHT; // always bright
    case FMT_GRAY:
        return 7 + offset;
    case FMT_BLACK_GRAY:
        return 0 + ANSI_ESC_BRIGHT; // always bright
    case FMT_COL_NONE:
    default:
        return -1;
    }
}

static IoResult fmt_start_style(IoWriter writer, FmtStyle style)
{
    size_t written = 0;
    if (style.bold)
    {
        written += TRY(IoResult, io_write_str(writer, str$("\033[1m")));
    }
    if (style.underline)
    {
        written += TRY(IoResult, io_write_str(writer, str$("\033[4m")));
    }

    int fg_id = fmt_color_id(style.fg_color);
    if (fg_id != -1)
    {
        written += TRY(IoResult, io_write_str(writer, str$("\033[")));
        written += TRY(IoResult, fmt_signed((Fmt){}, writer, fg_id + ANSI_ESC_FG));
        written += TRY(IoResult, io_write_str(writer, str$("m")));
    }

    int bg_id = fmt_color_id(style.bg_color);
    if (bg_id != -1)
    {
        written += TRY(IoResult, io_write_str(writer, str$("\033[")));
        written += TRY(IoResult, fmt_signed((Fmt){}, writer, bg_id + ANSI_ESC_BG));
        written += TRY(IoResult, io_write_str(writer, str$("m")));
    }

    return OK(IoResult, written);
}

static IoResult fmt_end_style(IoWriter writer)
{
    return io_write_str(writer, str$("\033[0m"));
}

IoResult fmt_any(Fmt self, IoWriter writer, Any value)
{
    size_t res = 0;

    if (self.style.has_style)
    {
        res = TRY(IoResult, fmt_start_style(writer, self.style));
    }

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

    if (self.style.has_style)
    {
        res += TRY(IoResult, fmt_end_style(writer));
    }

    return OK(IoResult, res);
}
