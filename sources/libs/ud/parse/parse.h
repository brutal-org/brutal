#pragma once
#include <brutal/parse.h>
#include <ud/ast.h>
#include <ud/ast/expr.h>

UdAst ud_parse(Lex *lex, Alloc *alloc);

void ud_parse_whitespace(Lex *lex);
bool ud_parse_expect_separator(Lex *lex, LexemeType type);
bool ud_parse_skip_separator(Lex *lex, LexemeType type);
bool ud_parse_is_separator(Lex *lex, LexemeType type);
bool ud_expect(Lex *lex, LexemeType type);
