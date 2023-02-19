#pragma once

#include <cc/lex/lexemes.h>
#include <brutal-parse>

// clang-format off

typedef enum
{
#define ITER(LEXEME) CLEX_##LEXEME,
    FOREACH_LEXEMES(ITER)
#undef ITER

    CLEX_COUNT,
} CLexemeType;

// clang-format on

static inline Str clex_to_str(LexemeType type)
{
    if (type >= 0 && type < CLEX_COUNT)
    {
        static const char *LEXEME_NAMES[] = {
#define ITER(LEXEME) #LEXEME,
            FOREACH_LEXEMES(ITER)
#undef ITER
        };

        return str$(LEXEME_NAMES[type]);
    }

    return lexeme_to_str(type);
}

Lex clex(Scan *scan, Alloc *alloc);
