#include "misc.h"
#include <stdbool.h>

enum format_type_width
{
    FMT_TYPE_CHAR,
    FMT_TYPE_SHORT,
    FMT_TYPE_INT,
    FMT_TYPE_LONG,
    FMT_TYPE_LONG_LONG,
    FMT_TYPE_SIZE,
    FMT_TYPE_PTRDIFF,
    FMT_TYPE_NONE,
};

enum format_type
{
    FMT_SCHAR,
    FMT_SHORT_INT,
    FMT_INT,
    FMT_LONG_INT,
    FMT_LONG_LONG_INT,
    FMT_UCHAR,
    FMT_SHORT_UINT,
    FMT_UINT,
    FMT_LONG_UINT,
    FMT_LONG_LONG_UINT,
    FMT_SIZE,
    FMT_PTR,
    FMT_PTRDIFF,
    FMT_STR,
    FMT_UEFI_STR,
    FMT_PERCENT,
    FMT_CHAR,
    FMT_NONE,
};

const unsigned FMT_FLAG_MINUS = 1;
const unsigned FMT_FLAG_PLUS = 2;
const unsigned FMT_FLAG_SPACE = 4;
const unsigned FMT_FLAG_BASE = 8;
const unsigned FMT_FLAG_ZERO = 16;

struct format
{
    unsigned flags;
    bool width_from_args;
    unsigned width;
    enum format_type_width type_width;
    enum format_type type;
    unsigned base;
    size_t size;
};

struct format_buffer
{
    uint16_t *buffer;
    size_t size;
    size_t count;
};

static void buffer_append(struct format_buffer *buffer, int code)
{
    if (buffer->count < buffer->size)
        buffer->buffer[buffer->count] = code;
    ++buffer->count;
}

static void buffer_copy_u16(
    struct format_buffer *buffer, const uint16_t *begin, const uint16_t *end)
{
    const size_t len = end - begin;
    const size_t rem =
        buffer->count < buffer->size
            ? buffer->size - buffer->count
            : 0;
    const size_t copy = len < rem ? len : rem;
    uint16_t *pos = buffer->buffer + buffer->count;

    for (size_t i = 0; i < copy; ++i)
        *pos++ = begin[i];
    buffer->count += len;
}

static void buffer_copy(
    struct format_buffer *buffer, const char *begin, const char *end)
{
    const size_t len = end - begin;
    const size_t rem =
        buffer->count < buffer->size
            ? buffer->size - buffer->count
            : 0;
    const size_t copy = len < rem ? len : rem;
    uint16_t *pos = buffer->buffer + buffer->count;

    for (size_t i = 0; i < copy; ++i)
        *pos++ = begin[i];
    buffer->count += len;
}

static void buffer_format_unsigned(
    struct format_buffer *buffer, struct format *config, unsigned long long v)
{
    char buf[32];
    char *pos = buf;

    do
    {
        const unsigned d = v % config->base;

        v = v / config->base;
        if (d < 10)
            *pos = d + '0';
        else
            *pos = d - 10 + 'A';
        ++pos;
    } while (v);

    for (char *l = buf, *r = pos - 1; l < r; ++l, --r)
    {
        const char c = *l;

        *l = *r;
        *r = c;
    }

    buffer_copy(buffer, buf, pos);
}

static void buffer_format_signed(
    struct format_buffer *buffer, struct format *config, long long v)
{
    if (v < 0)
    {
        buffer_append(buffer, '-');
        v = -v;
    }
    buffer_format_unsigned(buffer, config, v);
}

static const char *format_parse_flags(const char *str, struct format *config)
{
    while (*str)
    {
        bool is_flag = true;

        switch (*str)
        {
        case '-':
            config->flags = FMT_FLAG_MINUS;
            break;
        case '+':
            config->flags = FMT_FLAG_PLUS;
            break;
        case ' ':
            config->flags = FMT_FLAG_SPACE;
            break;
        case '#':
            config->flags = FMT_FLAG_BASE;
            break;
        case '0':
            config->flags = FMT_FLAG_ZERO;
            break;
        default:
            is_flag = false;
            break;
        }

        if (!is_flag)
            break;
        ++str;
    }
    return str;
}

static const char *format_parse_width(const char *str, struct format *config)
{
    if (*str == '*')
    {
        config->width_from_args = true;
        return str + 1;
    }

    while (isdigit(*str))
    {
        config->width = config->width * 10 + (*str - '0');
        ++str;
    }

    return str;
}

static const char *format_parse_type_width(const char *str, struct format *config)
{
    if (*str == 'z')
    {
        config->type_width = FMT_TYPE_SIZE;
        return str + 1;
    }

    if (*str == 't')
    {
        config->type_width = FMT_TYPE_PTRDIFF;
        return str + 1;
    }

    if (*str == 'h')
    {
        ++str;
        if (*str == 'h')
        {
            config->type_width = FMT_TYPE_CHAR;
            return str + 1;
        }
        config->type_width = FMT_TYPE_SHORT;
        return str;
    }

    if (*str == 'l')
    {
        ++str;
        if (*str == 'l')
        {
            config->type_width = FMT_TYPE_LONG_LONG;
            return str + 1;
        }
        config->type_width = FMT_TYPE_LONG;
        return str;
    }

    return str;
}

static enum format_type format_adjusted_type(enum format_type type, enum format_type_width width)
{
    if (type == FMT_INT)
    {
        switch (width)
        {
        case FMT_TYPE_CHAR:
            return FMT_SCHAR;
        case FMT_TYPE_SHORT:
            return FMT_SHORT_INT;
        case FMT_TYPE_LONG:
            return FMT_LONG_INT;
        case FMT_TYPE_LONG_LONG:
            return FMT_LONG_LONG_INT;
        case FMT_TYPE_SIZE:
            return FMT_SIZE;
        case FMT_TYPE_PTRDIFF:
            return FMT_PTRDIFF;
        default:
            return type;
        }
    }

    if (type == FMT_UINT)
    {
        switch (width)
        {
        case FMT_TYPE_CHAR:
            return FMT_UCHAR;
        case FMT_TYPE_SHORT:
            return FMT_SHORT_UINT;
        case FMT_TYPE_LONG:
            return FMT_LONG_UINT;
        case FMT_TYPE_LONG_LONG:
            return FMT_LONG_LONG_UINT;
        case FMT_TYPE_SIZE:
            return FMT_SIZE;
        case FMT_TYPE_PTRDIFF:
            return FMT_PTRDIFF;
        default:
            return type;
        }
    }

    return type;
}

static const char *format_parse_spec(const char *str, struct format *config)
{
    const char *pos = str;

    if (*pos == '%')
    {
        config->type = FMT_PERCENT;
        return pos + 1;
    }

    pos = format_parse_type_width(pos, config);

    switch (*pos)
    {
    case 'd':
    case 'i':
    case 'u':
        config->base = 10;
        break;
    case 'o':
        config->base = 8;
        break;
    case 'x':
    case 'X':
    case 'p':
        config->base = 16;
        break;
    }

    switch (*pos)
    {
    case 'd':
    case 'i':
        config->type = format_adjusted_type(FMT_INT, config->type_width);
        break;
    case 'u':
    case 'o':
    case 'x':
    case 'X':
        config->type = format_adjusted_type(FMT_UINT, config->type_width);
        break;
    case 'c':
        config->type = FMT_CHAR;
        break;
    case 's':
        config->type = FMT_STR;
        break;
    case 'w':
        config->type = FMT_UEFI_STR;
        break;
    case 'p':
        config->type = FMT_PTR;
        break;
    default:
        return str;
    }

    return pos + 1;
}

static int format_parse(const char *str, struct format *config)
{
    const char *pos = str;
    const char *end;

    config->flags = 0;
    config->width_from_args = false;
    config->width = 0;
    config->type_width = FMT_TYPE_NONE;
    config->type = FMT_NONE;
    config->base = 0;
    config->size = 0;

    pos = format_parse_flags(pos, config);
    pos = format_parse_width(pos, config);

    // The spec is the only mandatory part, so if it's empty it's a problem
    end = format_parse_spec(pos, config);
    if (pos == end)
        return -1;
    config->size = end - str;

    return 0;
}

int u16vsnprintf(uint16_t *buffer, size_t size, const char *fmt, va_list args)
{
    struct format_buffer buf = {buffer, size - 1, 0};
    const char *current = fmt;

    while (*current)
    {
        const char *start = current;
        const char *end = current;

        struct format config;
        unsigned long long uv;
        long long v;
        int ret;

        while (*end && *end != '%')
            ++end;

        buffer_copy(&buf, start, end);
        if (*end == '\0')
        {
            current = end;
            break;
        }

        ret = format_parse(end + 1, &config);
        if (ret < 0)
            return ret;
        current = end + 1 + config.size;

        if (config.width_from_args)
            config.width = (unsigned)va_arg(args, int);

        switch (config.type)
        {
        case FMT_STR:
        {
            const char *str = va_arg(args, const char *);
            buffer_copy(&buf, str, str + strlen(str));
            break;
        }
        case FMT_UEFI_STR:
        {
            const uint16_t *str = va_arg(args, const uint16_t *);
            buffer_copy_u16(&buf, str, str + u16strlen(str));
            break;
        }
        case FMT_CHAR:
        {
            int v = va_arg(args, int);
            buffer_append(&buf, v);
            break;
        }
        case FMT_PERCENT:
            buffer_append(&buf, u'%');
            break;
        case FMT_SCHAR:
        case FMT_SHORT_INT:
        case FMT_INT:
            v = va_arg(args, int);
            buffer_format_signed(&buf, &config, v);
            break;
        case FMT_LONG_INT:
            v = va_arg(args, long);
            buffer_format_signed(&buf, &config, v);
            break;
        case FMT_LONG_LONG_INT:
            v = va_arg(args, long long);
            buffer_format_signed(&buf, &config, v);
            break;
        case FMT_PTRDIFF:
            v = (long long)va_arg(args, ptrdiff_t);
            buffer_format_unsigned(&buf, &config, v);
            break;
        case FMT_UCHAR:
        case FMT_SHORT_UINT:
        case FMT_UINT:
            uv = va_arg(args, unsigned);
            buffer_format_unsigned(&buf, &config, uv);
            break;
        case FMT_LONG_UINT:
            uv = va_arg(args, unsigned long);
            buffer_format_unsigned(&buf, &config, uv);
            break;
        case FMT_LONG_LONG_UINT:
            uv = va_arg(args, unsigned long long);
            buffer_format_unsigned(&buf, &config, uv);
            break;
        case FMT_SIZE:
            uv = va_arg(args, size_t);
            buffer_format_unsigned(&buf, &config, uv);
            break;
        case FMT_PTR:
            uv = (unsigned long long)va_arg(args, void *);
            buffer_format_unsigned(&buf, &config, uv);
            break;
        default:
            return -1;
        }
    }

    if (buf.count < buf.size)
        buf.buffer[buf.count] = u'\0';
    else
        buf.buffer[buf.size] = u'\0';

    return (int)buf.count;
}

int u16snprintf(uint16_t *buffer, size_t size, const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = u16vsnprintf(buffer, size, fmt, args);
    va_end(args);
    return ret;
}

size_t u16strlen(const uint16_t *str)
{
    const uint16_t *pos = str;

    while (*pos++)
        ;
    return pos - str - 1;
}

int u16strcmp(const uint16_t *l, const uint16_t *r)
{
    while (*l == *r && *l != '\0')
    {
        ++l;
        ++r;
    }

    if (*l == *r)
        return 0;

    if (*l < *r)
        return -1;
    else
        return 1;
}

uint16_t *to_u16strncpy(uint16_t *dst, const char *src, size_t size)
{
    size_t i = 0;

    for (; i < size && src[i]; ++i)
        dst[i] = src[i];

    for (; i < size; ++i)
        dst[i] = '\0';

    return dst;
}
