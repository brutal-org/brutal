#pragma once

#include <cc/lex/lexer.h>
#include <brutal-ds>

typedef Vec(Lexemes) MacroArgsCodes;

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
} CProc;

void cproc_lex(Lex *result, Lex *source, CProc *ctx);

Lex cproc_file(Lex *source, Str filename, Alloc *alloc);

void cproc_gen_macro(Lex *out, Lex *source, CProc *ctx, CProcMacro *macro);

void cproc_parse_define(Lex *source, CProc *ctx);

CProcMacro *cproc_define(CProc *ctx, Str name, Str value);

void cproc_define_arg(CProcMacro *targ, Str name);
