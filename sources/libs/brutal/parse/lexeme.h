#pragma once

#include <brutal/text/str.h>

#define LEXEME_EOF (-1)
#define LEXEME_INVALID (-2)

typedef int LexemeType;

typedef struct
{
    LexemeType type;
    Str str;
} Lexeme;

static inline Str lexeme_to_str(LexemeType type)
{
    if (type == LEXEME_EOF)
    {
        return str$("EOF");
    }
    else
    {
        return str$("INVALID");
    }
}
