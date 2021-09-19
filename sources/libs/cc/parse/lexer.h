#pragma once

#include <brutal/parse.h>

#define FOREACH_LEXEMES(LEXEME) \
    LEXEME(WHITESPACE)          \
    LEXEME(COMMENT)             \
    LEXEME(ATOM)                \
    LEXEME(LPARENT)             \
    LEXEME(RPARENT)             \
    LEXEME(LBRACKET)            \
    LEXEME(RBRACKET)            \
    LEXEME(LBRACE)              \
    LEXEME(RBRACE)              \
    LEXEME(LCHEVRON)            \
    LEXEME(RCHEVRON)            \
    LEXEME(POUND)               \
    LEXEME(PLUS)                \
    LEXEME(MINUS)               \
    LEXEME(EQUAL)               \
    LEXEME(STAR)                \
    LEXEME(SLASH)               \
    LEXEME(PERCENT)             \
    LEXEME(AMPERSAND)           \
    LEXEME(BAR)                 \
    LEXEME(CIRCUMFLEX)          \
    LEXEME(TILDE)               \
    LEXEME(EXCLAMATION)         \
    LEXEME(DOT)

// clang-format off

typedef enum
{
#define ITER(LEXEME) CLEX_##LEXEME,
    FOREACH_LEXEMES(ITER)
#undef ITER

    CLEX_COUNT,
} CLexemeType;

// clang-format on

static inline Str c_lex_to_str(LexemeType type)
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

Lex c_lex(Scan *scan, Alloc *alloc);
