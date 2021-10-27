#pragma once

#include <brutal/text/str.h>

#define LEXEME_EOF (-1)
#define LEXEME_INVALID (-2)

typedef int LexemeType;

typedef struct
{
    LexemeType type;
    Str str;
    long integer_value;
} Lexeme;

#define lexeme$(TYPE) \
    (Lexeme) { (TYPE), str$(""), 0 }

#define lexeme_num$(TYPE, NUM) \
    (Lexeme) { (TYPE), str$(""), (NUM) }

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
