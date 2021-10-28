#pragma once

#include <cc/builder.h>
#include <cc/parse/lexer.h>

CDecl cparse_decl(Lex *lex, Alloc *alloc);

CType cparse_declarator(Lex *lex, CType type, Alloc *alloc);

CExpr cparse_expr(Lex *lex, Alloc *alloc);

CStmt cparse_stmt(Lex *lex, Alloc *alloc);

CType cparse_type(Lex *lex, Alloc *alloc);

CUnit cparse_unit(Lex *lex, Alloc *alloc);

CVal cparse_val(Lex *lex, Alloc *alloc);

/* --- Miscs ---------------------------------------------------------------- */

void cparse_whitespace(Lex *lex);

bool cparse_separator(Lex *lex, LexemeType type);
