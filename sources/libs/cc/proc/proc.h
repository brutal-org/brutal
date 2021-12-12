#pragma once

#include <brutal/ds/vec.h>
#include <cc/parse/lexer.h>

typedef struct
{
    Str name;
    Lexemes source;
    Vec(Str) args;
} CProcMacro;

typedef struct
{
    Lexemes source;
    Str filename;
    Vec(CProcMacro) macros;
    Vec(Str) files_included;
    Vec(Str) include_path;
    Vec(Str) inside_macro;
    Alloc *alloc;
} CProcContext;

void cproc_parse_token(Lex *result, Lex *source, CProcContext *context);

void cproc_lex(Lex *out, Lex *source, CProcContext *ctx);

Lex cproc_file(Lex *source, Str filename, Alloc *alloc);
