#pragma once

#include <brutal/parse.h>

#define FOREACH_LEXEMES(LEXEME) \
    LEXEME(WHITESPACE)          \
    LEXEME(COMMENT)             \
    LEXEME(IDENT)               \
    LEXEME(NIL)                 \
    LEXEME(BOOL)                \
    LEXEME(U8)                  \
    LEXEME(U16)                 \
    LEXEME(U32)                 \
    LEXEME(U64)                 \
    LEXEME(INT)                 \
    LEXEME(UINT)                \
    LEXEME(STRUCT)              \
    LEXEME(SEMICOLON)           \
    LEXEME(FUNC)                \
    LEXEME(LET)                 \
    LEXEME(THIN_ARROW)          \
    LEXEME(COLON)               \
    LEXEME(DOUBLE_EQUAL)        \
    LEXEME(EQUAL)               \
    LEXEME(LCBRACE)             \
    LEXEME(RCBRACE)             \
    LEXEME(LPAREN)              \
    LEXEME(RPAREN)              \
    LEXEME(COMMA)               \
    LEXEME(DOT)                 \
    LEXEME(RBRACKET)            \
    LEXEME(LBRACKET)            \
    LEXEME(SLASH)               \
    LEXEME(MINUS)               \
    LEXEME(PLUS)                \
    LEXEME(STAR)                \
    LEXEME(INTEGER)             \
    LEXEME(STRING)              \
    LEXEME(CHARACTER)

// clang-format off

typedef enum
{
#define ITER(LEXEME) UDLEX_##LEXEME,
    FOREACH_LEXEMES(ITER)
#undef ITER

    UDLEX_COUNT,
} UdLexemeType;

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

Lex ud_lex(Scan *scan, Alloc *alloc);
