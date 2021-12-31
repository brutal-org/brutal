#pragma once

#include <brutal/alloc/base.h>
#include <brutal/text/rune.h>

StrFix8 utf8_from_rune(Rune rune);

size_t utf8_rune_len(Rune rune);

Rune utf8_to_rune(Str str);

size_t utf8_byte_len(uint8_t str);

uint8_t *utf8_str_to_cstr(Str str, Alloc *alloc);

bool uft8_next_rune(Str *str, Rune *rune);
