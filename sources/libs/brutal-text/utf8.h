#pragma once

#include <brutal-alloc>

#include "rune.h"
#include "str.h"

StrFix8 utf8_from_rune(Rune rune);

size_t utf8_rune_len(Rune rune);

Rune utf8_to_rune(Str str);

size_t utf8_byte_len(uint8_t str);

uint8_t *utf8_str_to_cstr(Str str, Alloc *alloc);

bool uft8_next_rune(Str *str, Rune *rune);

typedef struct
{
    size_t len;
    uint8_t buffer[5];
} Utf8Decoder;

static inline bool utf8_decode(Utf8Decoder *self, uint8_t byte, Rune *rune)
{
    self->buffer[self->len++] = byte;

    if (utf8_byte_len(self->buffer[0]) == self->len)
    {
        *rune = utf8_to_rune(str_n$(self->len, (const char *)self->buffer));
        return true;
    }

    return false;
}
