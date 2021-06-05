#pragma once

#include <library/base/std.h>
#include <library/text/str.h>

typedef uint32_t rune_t;

str_fix8_t rune_to_utf8(rune_t rune);

size_t rune_length_utf8(rune_t rune);

rune_t utf8_to_rune(str_t str);

size_t utf8_length(uint8_t str);
