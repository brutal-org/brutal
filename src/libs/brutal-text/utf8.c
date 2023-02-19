#include "utf8.h"

StrFix8 utf8_from_rune(Rune rune)
{
    if (rune <= 0x7F)
    {
        return (StrFix8){
            .len = 1,
            .buf = {
                (uint8_t)rune,
            },
        };
    }
    else if (rune <= 0x07FF)
    {
        return (StrFix8){
            .len = 2,
            .buf = {
                (uint8_t)(((rune >> 6) & 0x1F) | 0xC0),
                (uint8_t)(((rune >> 0) & 0x3F) | 0x80),
            },
        };
    }
    else if (rune <= 0xFFFF)
    {
        return (StrFix8){
            .len = 3,
            .buf = {
                (uint8_t)(((rune >> 12) & 0x0F) | 0xE0),
                (uint8_t)(((rune >> 6) & 0x3F) | 0x80),
                (uint8_t)(((rune >> 0) & 0x3F) | 0x80),
            },
        };
    }
    else if (rune <= 0x10FFFF)
    {
        return (StrFix8){
            .len = 4,
            .buf = {
                (uint8_t)(((rune >> 18) & 0x07) | 0xF0),
                (uint8_t)(((rune >> 12) & 0x3F) | 0x80),
                (uint8_t)(((rune >> 6) & 0x3F) | 0x80),
                (uint8_t)(((rune >> 0) & 0x3F) | 0x80),
            },
        };
    }
    else
    {
        return (StrFix8){
            .len = 3,
            .buf = {0xEF, 0xBF, 0xBD},
        };
    }
}

size_t utf8_rune_len(Rune rune)
{
    if (rune <= 0x7F)
    {
        return 1;
    }
    else if (rune <= 0x07FF)
    {
        return 2;
    }
    else if (rune <= 0xFFFF)
    {
        return 3;
    }
    else if (rune <= 0x10FFFF)
    {
        return 4;
    }
    else
    {
        return 0;
    }
}

static bool utf8_valid(Str str)
{
    return !str_empty(str) && str.len >= utf8_byte_len(str.buf[0]);
}

Rune utf8_to_rune(Str str)
{
    if (!utf8_valid(str))
    {
        return U'�';
    }

    if ((str.buf[0] & 0xf8) == 0xf0)
    {
        return ((0x07 & str.buf[0]) << 18) |
               ((0x3f & str.buf[1]) << 12) |
               ((0x3f & str.buf[2]) << 6) |
               ((0x3f & str.buf[3]));
    }
    else if ((str.buf[0] & 0xf0) == 0xe0)
    {
        return ((0x0f & str.buf[0]) << 12) |
               ((0x3f & str.buf[1]) << 6) |
               ((0x3f & str.buf[2]));
    }
    else if ((str.buf[0] & 0xe0) == 0xc0)
    {
        return ((0x1f & str.buf[0]) << 6) |
               ((0x3f & str.buf[1]));
    }
    else
    {
        return str.buf[0];
    }
}

size_t utf8_byte_len(uint8_t first)
{
    if ((first & 0xf8) == 0xf0)
    {
        return 4;
    }
    else if ((first & 0xf0) == 0xe0)
    {
        return 3;
    }
    else if ((first & 0xe0) == 0xc0)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

uint8_t *utf8_str_to_cstr(Str str, Alloc *alloc)
{
    uint8_t *cstr = (uint8_t *)alloc_malloc(alloc, str.len + 1);

    mem_cpy(cstr, str.buf, str.len);
    cstr[str.len] = '\0';

    return cstr;
}

bool uft8_next_rune(Str *str, Rune *rune)
{
    if (!utf8_valid(*str))
    {
        return false;
    }

    uint8_t len = utf8_byte_len(str->buf[0]);
    *rune = utf8_to_rune(*str);

    str->buf += len;
    str->len -= len;

    return true;
}
