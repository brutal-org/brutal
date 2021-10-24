#pragma once

#include <ud/ast/sexpr.h>

/* --- Keywords ------------------------------------------------------------- */

#define UD_BUILTIN_DEFINE "ud-builtin-define"
UdExpr ud_builtin_define(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_LAMBDA "ud-builtin-lambda"
UdExpr ud_builtin_lambda(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_BLOCK "ud-builtin-block"
UdExpr ud_builtin_block(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_IF "ud-builtin-if"
UdExpr ud_builtin_if(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_WHILE "ud-builtin-while"
UdExpr ud_builtin_while(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_FOR "ud-builtin-for"
UdExpr ud_builtin_for(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_MATCH "ud-builtin-match"
UdExpr ud_builtin_match(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_ASSERT "ud-builtin-assert"
UdExpr ud_builtin_assert(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_EXPORT "ud-builtin-export"
UdExpr ud_builtin_export(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_IMPORT "ud-builtin-import"
UdExpr ud_builtin_import(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_TYPE "ud-builtin-type"
UdExpr ud_builtin_type(UdExpr args, UdExpr *env, Alloc *alloc);

/* --- Operators ------------------------------------------------------------ */

#define UD_BUILTIN_QUOTE "ud-builtin-quote"
UdExpr ud_builtin_quote(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_PAIR "ud-builtin-pair"
UdExpr ud_builtin_pair(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_DOT "ud-builtin-dot"
UdExpr ud_builtin_dot(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_REF "ud-builtin-ref"
UdExpr ud_builtin_ref(UdExpr args, UdExpr *env, Alloc *alloc);

// Math operators

#define UD_BUILTIN_ADD "ud-builtin-add"
UdExpr ud_builtin_add(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_SUB "ud-builtin-sub"
UdExpr ud_builtin_SUB(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_MULT "ud-builtin-mult"
UdExpr ud_builtin_mult(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_DIV "ud-builtin-div"
UdExpr ud_builtin_div(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_POW "ud-builtin-pow"
UdExpr ud_builtin_pow(UdExpr args, UdExpr *env, Alloc *alloc);

// Comparaison operators

#define UD_BUILTIN_EQ "ud-builtin-eq"
UdExpr ud_builtin_eq(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_GT "ud-builtin-gt"
UdExpr ud_builtin_gt(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_LT "ud-builtin-lt"
UdExpr ud_builtin_lt(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_GTEQ "ud-builtin-gteq"
UdExpr ud_builtin_gteq(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_LTEQ "ud-builtin-lteq"
UdExpr ud_builtin_lteq(UdExpr args, UdExpr *env, Alloc *alloc);

// Logical operators

#define UD_BUILTIN_AND "ud-builtin-and"
UdExpr ud_builtin_and(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_OR "ud-builtin-or"
UdExpr ud_builtin_or(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_NOT "ud-builtin-not"
UdExpr ud_builtin_not(UdExpr args, UdExpr *env, Alloc *alloc);

// Binary logical operators

#define UD_BUILTIN_BIN_AND "ud-builtin-bin-and"
UdExpr ud_builtin_bin_and(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_BIN_OR "ud-builtin-bin-or"
UdExpr ud_builtin_bin_or(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_BIN_NOT "ud-builtin-bin-not"
UdExpr ud_builtin_bin_not(UdExpr args, UdExpr *env, Alloc *alloc);

// Shift operators

#define UD_BUILTIN_lshift "ud-builtin-lshift"
UdExpr ud_builtin_lshift(UdExpr args, UdExpr *env, Alloc *alloc);

#define UD_BUILTIN_rshift "ud-builtin-rshift"
UdExpr ud_builtin_rshift(UdExpr args, UdExpr *env, Alloc *alloc);
