#pragma once

#include <brutal/ds/vec.h>
#include <brutal/parse/lexeme.h>
#include <brutal/parse/scan.h>

typedef struct
{
    Str message;
    Lexeme lexeme;
} LexError;

typedef struct
{
    Vec(Lexeme) lexemes;
    int head;

    bool has_error;
    LexError error;
} Lex;

typedef LexemeType LexFn(Scan *scan);

Lex lex(Scan *scan, LexFn *fn, Alloc *alloc);

void lex_deinit(Lex *self);

bool lex_ended(Lex *self);

Lexeme lex_peek(Lex *self, int offset);

Lexeme lex_curr(Lex *self);

Lexeme lex_next(Lex *self);

void lex_throw(Lex *self, Str message);

bool lex_expect(Lex *self, int lexeme);

bool lex_ok(Lex *self);
