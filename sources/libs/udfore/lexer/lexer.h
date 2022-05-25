#pragma once

#include <brutal-parse>

#include "lexeme.h"

// clang-format off

typedef enum
{
#define ITER(LEXEME) UDLEX_##LEXEME,
    FOREACH_LEXEMES(ITER)
#undef ITER

    UDLEX_COUNT,
} UDLexemeType;

// clang-format on

static inline Str udlex_to_str(LexemeType type)
{
    if (type >= 0 && type < UDLEX_COUNT)
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

Lex udlex(Scan *scan, Alloc *alloc);
