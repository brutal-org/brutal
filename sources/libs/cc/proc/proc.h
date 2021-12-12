#pragma once

#include <brutal/ds/vec.h>
#include <cc/parse/lexer.h>

typedef struct
{
    Str name;
    Lexemes lexemes;
    Vec(Str) args;
} CProcMacro;

typedef struct
{
    Lexemes lexemes;
    Str filename;
    Vec(CProcMacro) macros;
    Vec(Str) files_included;
    Vec(Str) include_path;
    Vec(Str) inside_macro;
} CProcContext;

Lex cproc_file(Lex *lex, Str filename, Alloc *alloc);
