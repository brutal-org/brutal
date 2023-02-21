#pragma once

#include <cc/builder.h>
#include <cc/lex/lexer.h>

CDecl cparse_decl(Lex *lex, CUnit *context, Alloc *alloc);

CDeclarator cparse_declarator(Lex *lex, Alloc *alloc);

CExpr cparse_expr(Lex *lex, int pre, CUnit *context, Alloc *alloc);

CStmt cparse_stmt(Lex *lex, CUnit *context, Alloc *alloc);

void cparse_func_params(Lex *lex, CType *type, Alloc *alloc);

CType cparse_type(Lex *lex, Alloc *alloc);

CUnit cparse_unit(Lex *lex, Alloc *alloc);

CVal cparse_val(Lex *lex);

/* --- Miscs ---------------------------------------------------------------- */

void cparse_whitespace(Lex *lex);

bool cparse_expect_separator(Lex *lex, LexemeType type);

bool cparse_skip_separator(Lex *lex, LexemeType type);

bool cparse_is_separator(Lex *lex, LexemeType type);

Lexeme cparse_peek_separator(Lex *lex, int offset);
