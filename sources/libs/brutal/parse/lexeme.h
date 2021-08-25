#pragma once

#include <brutal/text/str.h>

#define LEXEME_EOF (-1)

typedef int LexemeType;

typedef struct
{
    LexemeType type;
    Str str;
} Lexeme;
