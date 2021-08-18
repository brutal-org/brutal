#pragma once

#include <brutal/alloc/base.h>
#include <brutal/text/rune.h>

StrFix8 rune_to_utf8(Rune rune);

size_t rune_length_utf8(Rune rune);

Rune utf8_to_rune(Str str);

size_t utf8_length(uint8_t str);

uint8_t *str_to_cstr_utf8(Str str, Alloc *alloc);
