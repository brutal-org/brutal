#pragma once

// macro parsing and generation

#include <cc/proc/proc.h>

typedef Vec(Lexemes) MacroArgsCodes;

void cproc_gen_macro(Lex *out, Lex *source, CProcContext *ctx, CProcMacro *macro);

void cproc_parse_define(Lex *source, CProcContext *ctx);
