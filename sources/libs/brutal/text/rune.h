#pragma once

#include <brutal/base/std.h>
#include <brutal/text/str.h>

typedef uint32_t Rune;

StrFix8 rune_to_utf8(Rune rune);

size_t rune_length_utf8(Rune rune);

Rune utf8_to_rune(Str str);

size_t utf8_length(uint8_t str);
