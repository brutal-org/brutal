#pragma once

#include <brutal-parse>

typedef enum
{
    CASE_DEFAULT,
    CASE_CAMEL,
    CASE_CAPITAL,
    CASE_CONSTANT,
    CASE_DOT,
    CASE_HEADER,
    CASE_NO,
    CASE_PARAM,
    CASE_PASCAL,
    CASE_PATH,
    CASE_SENTENCE,
    CASE_SNAKE,
    CASE_TITLE,
    CASE_SWAP,
    CASE_LOWER,
    CASE_LOWER_FIRST,
    CASE_UPPER,
    CASE_UPPER_FIRST,
    CASE_SPONGE,
} Case;

Buf case_to_default(Str str, Alloc *alloc);

Buf case_to_camel(Str str, Alloc *alloc);

Buf case_to_capital(Str str, Alloc *alloc);

Buf case_to_constant(Str str, Alloc *alloc);

Buf case_to_dot(Str str, Alloc *alloc);

Buf case_to_header(Str str, Alloc *alloc);

Buf case_to_no(Str str, Alloc *alloc);

Buf case_to_param(Str str, Alloc *alloc);

Buf case_to_pascal(Str str, Alloc *alloc);

Buf case_to_path(Str str, Alloc *alloc);

Buf case_to_sentence(Str str, Alloc *alloc);

Buf case_to_snake(Str str, Alloc *alloc);

Buf case_to_title(Str str, Alloc *alloc);

Buf case_to_swap(Str str, Alloc *alloc);

Buf case_to_lower(Str str, Alloc *alloc);

Buf case_to_lower_first(Str str, Alloc *alloc);

Buf case_to_upper(Str str, Alloc *alloc);

Buf case_to_upper_first(Str str, Alloc *alloc);

Buf case_to_sponge(Str str, Alloc *alloc);

Buf case_change(Case c, Str str, Alloc *alloc);

Str case_change_str(Case c, Str str, Alloc *alloc);

Case case_parse(Scan *scan);
