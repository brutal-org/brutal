#pragma once
#include <brutal/io.h>
#include <brutal/parse.h>
#include <ud/ast.h>
#include <ud/ast/expr.h>

UdAstNode ud_parse(Lex *lex, Alloc *alloc);
UdAst ud_parse_file(Lex *lex, Alloc *alloc);

void ud_parse_whitespace(Lex *lex);

bool ud_parse_expect_separator(Lex *lex, LexemeType type);

bool ud_parse_skip_separator(Lex *lex, LexemeType type);

bool ud_parse_is_separator(Lex *lex, LexemeType type);

bool ud_expect(Lex *lex, LexemeType type);

UdAstNode ud_parse_decl(Lex *lex, Alloc *alloc);

UdAstNode ud_parse_expr(Lex *lex, Alloc *alloc);

UdFuncCall ud_parse_func_call(Lex *lex, Alloc *alloc);

void ud_print_expr(Emit *emit, Alloc *alloc, UdExpr expr);
void ud_print_stmt(Emit *, Alloc *, UdExpr);

bool ud_get_error();
void ud_set_error(bool value);
