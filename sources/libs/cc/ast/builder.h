#pragma once

#include <cc/ast/decl.h>
#include <cc/ast/expr.h>
#include <cc/ast/stmt.h>
#include <cc/ast/type.h>
#include <cc/ast/val.h>

/* --- CDecl ---------------------------------------------------------------- */

CDecl cdecl_type(Str name, CType type, Alloc *alloc)
{
    return (CDecl){
        .type = CDECL_TYPE,
        .name = str_dup(name, alloc),
        .type_.type = type,
    };
}

CDecl cdecl_var(Str name, CType type, CExpr expr, Alloc *alloc)
{
    return (CDecl){
        .type = CDECL_VAR,
        .name = str_dup(name, alloc),
        .var_ = {
            .type = type,
            .expr = expr,
        },
    };
}

CDecl cdecl_func(Str name, CType type, CStmt body, Alloc *alloc)
{
    return (CDecl){
        .type = CDECL_VAR,
        .name = str_dup(name, alloc),
        .func_ = {
            .type = type,
            .body = body,
        },
    };
}

/* --- CExpr ---------------------------------------------------------------- */

CExpr cexpr_empty(void)
{
    return (CExpr){
        .type = CEXPR_EMPTY,
    };
}

CExpr cexpr_constant(CVal val)
{
    return (CExpr){
        .type = CEXPR_CONSTANT,
        .constant_ = val,
    };
}

/* --- CStmt ---------------------------------------------------------------- */

CStmt cstmt_empty(void)
{
    return (CStmt){
        .type = CSTMT_EMPTY,
    };
}

CStmt cstmt_decl(CDecl decl, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_EMPTY,
        .decl_.decl = alloc_move(alloc, decl),
    };
}

CStmt cstmt_expr(CExpr expr)
{
    return (CStmt){
        .type = CSTMT_BLOCK,
        .expr_.expr = expr,
    };
}

CStmt cstmt_block(Alloc *alloc)
{
    CStmt stmt = {
        .type = CSTMT_BLOCK,
    };

    vec_init(&stmt.block_.stmts, alloc);

    return stmt;
}

CStmt cstmt_if(CExpr expr, CStmt stmt_true, CStmt stmt_false, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_IF,
        .if_ = {
            .expr = expr,
            .stmt_true = alloc_move(alloc, stmt_true),
            .stmt_false = alloc_move(alloc, stmt_false),
        },
    };
}

CStmt cstmt_for(CStmt init_stmt, CExpr cond_expr, CExpr iter_expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_FOR,
        .for_ = {
            .init_stmt = alloc_move(alloc, init_stmt),
            .cond_expr = cond_expr,
            .iter_expr = iter_expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

CStmt cstmt_while(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_WHILE,
        .while_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

CStmt cstmt_do(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_DO,
        .while_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

CStmt cstmt_switch(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_SWITCH,
        .while_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

CStmt cstmt_return(CExpr expr)
{
    return (CStmt){
        .type = CSTMT_RETURN,
        .return_ = {
            .expr = expr,
        },
    };
}

CStmt cstmt_goto(Str str, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_GOTO,
        .goto_.label = str_dup(str, alloc),
    };
}

CStmt cstmt_case(CExpr expr)
{
    return (CStmt){
        .type = CSTMT_CASE,
        .case_.expr = expr,
    };
}

/* --- CType ---------------------------------------------------------------- */

static inline CType ctype_void(Str name, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_VOID,
        .name = str_dup(name, alloc),
    };
}

static inline CType ctype_bool(Str name, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_BOOL,
        .name = str_dup(name, alloc),
    };
}

static inline CType ctype_ptr(Str name, CType subtype, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_PTR,
        .name = str_dup(name, alloc),
        .ptr_.subtype = alloc_move(alloc, subtype),
    };
}

static inline CType ctype_array(Str name, CType subtype, int size, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_ARRAY,
        .name = str_dup(name, alloc),
        .array_ = {
            .subtype = alloc_move(alloc, subtype),
            .size = size,
        },
    };
}

static inline CType ctype_signed(Str name, int precision, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_SIGNED,
        .name = str_dup(name, alloc),
        .signed_.precision = precision,
    };
}

static inline CType ctype_unsigned(Str name, int precision, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_UNSIGNED,
        .name = str_dup(name, alloc),
        .unsigned_.precision = precision,
    };
}

static inline CType ctype_float(Str name, int precision, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_FLOAT,
        .name = str_dup(name, alloc),
        .float_.precision = precision,
    };
}

static inline CType ctype_struct(Str name, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_STRUCT,
        .name = str_dup(name, alloc),
    };

    vec_init(&type.struct_.members, alloc);

    return type;
}

static inline CType ctype_union(Str name, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_UNION,
        .name = str_dup(name, alloc),
    };

    vec_init(&type.union_.members, alloc);

    return type;
}

static inline void ctype_member(CType *self, Str name, CType type, Alloc *alloc)
{
    CTypeMember member = {
        .name = str_dup(name, alloc),
        .type = type,
    };

    if (self->type == CTYPE_STRUCT || self->type == CTYPE_UNION)
    {
        vec_push(&self->struct_.members, member);
    }
    else if (self->type == CTYPE_FUNC)
    {
        vec_push(&self->func_.params, member);
    }
    else
    {
        panic$("Only structs, unions or funcs can have members!");
    }
}

static inline CType ctype_enum(Str name, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_ENUM,
        .name = str_dup(name, alloc),
    };

    vec_init(&type.enum_.constants, alloc);

    return type;
}

static inline void ctype_constant(CType *self, Str name, CVal val, Alloc *alloc)
{
    CTypeConstant constant = {
        .name = str_dup(name, alloc),
        .value = val,
    };

    if (self->type == CTYPE_ENUM)
    {
        vec_push(&self->enum_.constants, constant);
    }
    else
    {
        panic$("Only enums can have constants!");
    }
}

/* --- CVal  ---------------------------------------------------------------- */

static inline CVal cval_signed(CSigned v)
{
    return (CVal){
        .type = CVAL_SIGNED,
        .signed_ = v,
    };
}

static inline CVal cval_unsigned(CUnsigned v)
{
    return (CVal){
        .type = CVAL_SIGNED,
        .unsigned_ = v,
    };
}

static inline CVal cval_float(double v)
{
    return (CVal){
        .type = CVAL_FLOAT,
        .float_ = v,
    };
}

static inline CVal cval_string(Str str, Alloc *alloc)
{
    return (CVal){
        .type = CVAL_STRING,
        .string_ = str_dup(str, alloc),
    };
}
