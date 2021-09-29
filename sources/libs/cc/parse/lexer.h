#pragma once

#include <brutal/parse.h>

#define FOREACH_LEXEMES(LEXEME) \
    LEXEME(WHITESPACE)          \
    LEXEME(COMMENT)             \
    LEXEME(ATOM)                \
    LEXEME(AUTO)                \
    LEXEME(BREAK)               \
    LEXEME(CASE)                \
    LEXEME(CHAR)                \
    LEXEME(CONST)               \
    LEXEME(CONTINUE)            \
    LEXEME(DEFAULT)             \
    LEXEME(DO)                  \
    LEXEME(DOUBLE)              \
    LEXEME(ELSE)                \
    LEXEME(ENUM)                \
    LEXEME(EXTERN)              \
    LEXEME(FLOAT)               \
    LEXEME(FOR)                 \
    LEXEME(GOTO)                \
    LEXEME(IF)                  \
    LEXEME(INLINE)              \
    LEXEME(INT)                 \
    LEXEME(LONG)                \
    LEXEME(REGISTER)            \
    LEXEME(RESTRICT)            \
    LEXEME(RETURN)              \
    LEXEME(SHORT)               \
    LEXEME(SIGNED)              \
    LEXEME(SIZEOF)              \
    LEXEME(STATIC)              \
    LEXEME(STRUCT)              \
    LEXEME(SWITCH)              \
    LEXEME(TYPEDEF)             \
    LEXEME(UNION)               \
    LEXEME(UNSIGNED)            \
    LEXEME(VOID)                \
    LEXEME(VOLATILE)            \
    LEXEME(WHILE)               \
    LEXEME(ALIGNAS)             \
    LEXEME(ALIGNOF)             \
    LEXEME(ATOMIC)              \
    LEXEME(BOOL)                \
    LEXEME(GENERIC)             \
    LEXEME(NORETURN)            \
    LEXEME(STATIC_ASSERT)       \
    LEXEME(THREAD_LOCAL)        \
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
    LEXEME(PLUSPLUS)            \
    LEXEME(MINUS)               \
    LEXEME(MINUSMINUS)          \
    LEXEME(EQUALEQUAL)          \
    LEXEME(EQUAL)               \
    LEXEME(STAR)                \
    LEXEME(SLASH)               \
    LEXEME(PERCENT)             \
    LEXEME(AMPERSANDAMPERSAND)  \
    LEXEME(AMPERSAND)           \
    LEXEME(BAR)                 \
    LEXEME(CIRCUMFLEX)          \
    LEXEME(TILDE)               \
    LEXEME(EXCLAMATION)         \
    LEXEME(DOT)                 \
    LEXEME(ARROW)               \
    LEXEME(SEMICOLON)

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
