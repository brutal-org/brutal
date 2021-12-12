#pragma once

#include <brutal/ds/vec.h>
#include <brutal/parse/lexeme.h>
#include <brutal/parse/scan.h>

typedef struct
{
    Str message;
    Lexeme lexeme;
} LexError;

typedef Vec(Lexeme) Lexemes;

typedef struct
{
    Lexemes lexemes;
    int head;

    bool has_error;
    LexError error;
} Lex;

typedef LexemeType LexFn(Scan *scan);

typedef Str LexToStrFn(LexemeType type);

Lex lex(Scan *scan, LexFn *fn, Alloc *alloc);

Lex lex_from_lexeme(Lexemes *from);

void lex_deinit(Lex *self);

bool lex_ended(Lex *self);

Lexeme lex_peek(Lex *self, int offset);

Lexeme lex_curr(Lex *self);

Lexeme lex_next(Lex *self);

LexemeType lex_peek_type(Lex *self, int offset);

LexemeType lex_curr_type(Lex *self);

LexemeType lex_next_type(Lex *self);

bool lex_skip_type(Lex *lex, LexemeType type);

void lex_dump(Lex *self, LexToStrFn fn);

void lex_throw(Lex *self, Str message);

bool lex_expect(Lex *self, LexemeType type);

bool lex_ok(Lex *self);
