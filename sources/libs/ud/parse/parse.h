#pragma once
#include <brutal/io.h>
#include <brutal/parse.h>
#include <ud/ast.h>
#include <ud/ast/expr.h>

UdAst ud_parse(Lex *lex, Alloc *alloc);

void ud_parse_whitespace(Lex *lex);

bool ud_parse_expect_separator(Lex *lex, LexemeType type);

bool ud_parse_skip_separator(Lex *lex, LexemeType type);

bool ud_parse_is_separator(Lex *lex, LexemeType type);

bool ud_expect(Lex *lex, LexemeType type);

UdAstNode ud_parse_decl(Lex *lex, Alloc *alloc);

UdAstNode ud_parse_expr(Lex *lex, Alloc *alloc);

void ud_print_expr(Emit *emit, Alloc *alloc, UdExpr expr);
void ud_print_stmt(Emit *, Alloc *, UdStmt);

bool ud_get_error();
void ud_set_error();