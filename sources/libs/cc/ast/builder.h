#pragma once

#include <cc/ast/decl.h>
#include <cc/ast/expr.h>
#include <cc/ast/stmt.h>
#include <cc/ast/type.h>
#include <cc/ast/unit.h>
#include <cc/ast/val.h>

/* --- CDecl ---------------------------------------------------------------- */

static inline CDecl cdecl_type(Str name, CType type, Alloc *alloc)
{
    return (CDecl){
        .type = CDECL_TYPE,
        .name = str_dup(name, alloc),
        .type_.type = type,
    };
}

static inline CDecl cdecl_var(Str name, CType type, CExpr expr, Alloc *alloc)
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

static inline CDecl cdecl_func(Str name, CType type, CStmt body, Alloc *alloc)
{
    return (CDecl){
        .type = CDECL_FUNC,
        .name = str_dup(name, alloc),
        .func_ = {
            .type = type,
            .body = body,
        },
    };
}

/* --- CExpr ---------------------------------------------------------------- */

static inline CExpr cexpr_empty(void)
{
    return (CExpr){
        .type = CEXPR_EMPTY,
    };
}

static inline CExpr cexpr_constant(CVal val)
{
    return (CExpr){
        .type = CEXPR_CONSTANT,
        .constant_ = val,
    };
}

static inline CExpr cexpr_identifier(Str ident, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_IDENTIFIER,
        .identifier_ = str_dup(ident, alloc)};
}

static inline CExpr cexpr_call(Alloc *alloc, CExpr name)
{
    CExpr call;
    call.type = CEXPR_CALL;
    call.call_.expr = alloc_move(alloc, name);

    vec_init(&call.call_.args, alloc);

    return call;
}

static inline void cexpr_add_arg(CExpr *self, CExpr arg)
{
    if (self->type != CEXPR_CALL)
    {
        panic$("cexpr add arg should only be called with a call expression");
    }

    vec_push(&self->call_.args, arg);
}

static inline CExpr cexpr_infix(CExpr left, COp type, CExpr right, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_INFIX,
        .infix_ = {
            .op = type,
            .lhs = alloc_move(alloc, left),
            .rhs = alloc_move(alloc, right),
        },
    };
}

static inline CExpr cexpr_prefix(COp type, CExpr left, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_PREFIX,
        .prefix_ = {
            .op = type,
            .expr = alloc_move(alloc, left),
        },
    };
}

static inline CExpr cexpr_postfix(CExpr left, COp type, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_POSTFIX,
        .postfix_ = {
            .op = type,
            .expr = alloc_move(alloc, left),
        },
    };
}

static inline CExpr cexpr_cast(CExpr expr, CType type, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_CAST,
        .cast_ =
            {
                .type = type,
                .expr = alloc_move(alloc, expr),
            },
    };
}

static inline CExpr cexpr_ternary(CExpr cond, CExpr etrue, CExpr efalse, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_TERNARY,
        .ternary_ =
            {
                .expr_cond = alloc_move(alloc, cond),
                .expr_true = alloc_move(alloc, etrue),
                .expr_false = alloc_move(alloc, efalse),
            },
    };
}

static inline CExpr cexpr_array_initializer(Alloc *alloc)
{
    CExpr result = {
        .type = CEXPR_ARRAY_INITIALIZER,
    };

    vec_init(&result.array_initializer_.initializer, alloc);

    return result;
}

static inline CExpr cexpr_struct_initializer(Alloc *alloc)
{
    CExpr result = {
        .type = CEXPR_STRUCT_INITIALIZER,
    };

    vec_init(&result.struct_initializer_.initializer, alloc);

    return result;
}

static inline void cexpr_initializer_push_element(CExpr *target, CExpr designator, CExpr expr, Alloc *alloc)
{
    if (target->type == CEXPR_STRUCT_INITIALIZER || target->type == CEXPR_ARRAY_INITIALIZER)
    {
        CInitializer initializer = {.designator = alloc_move(alloc, designator), .initializer = alloc_move(alloc, expr)};
        vec_push(&target->struct_initializer_.initializer, initializer);
    }
    else
    {
        panic$("cepxr_initializer_push_element must be used with array initializer or struct initializer");
    }
}

/* --- CStmt ---------------------------------------------------------------- */

static inline CStmt cstmt_empty(void)
{
    return (CStmt){
        .type = CSTMT_EMPTY,
    };
}

static inline CStmt cstmt_decl(CDecl decl, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_EMPTY,
        .decl_.decl = alloc_move(alloc, decl),
    };
}

static inline CStmt cstmt_expr(CExpr expr)
{
    return (CStmt){
        .type = CSTMT_EXPR,
        .expr_.expr = expr,
    };
}

static inline CStmt cstmt_block(Alloc *alloc)
{
    CStmt stmt = {
        .type = CSTMT_BLOCK,
    };

    vec_init(&stmt.block_.stmts, alloc);

    return stmt;
}

static inline void cstmt_block_add(CStmt *self, CStmt statement)
{
    if (self->type != CSTMT_BLOCK)
    {
        panic$("cstmt_block_add should only be called with block");
    }

    vec_push(&self->block_.stmts, statement);
}

static inline CStmt cstmt_if(CExpr expr, CStmt stmt_true, Alloc *alloc)
{
    CStmt stmt_false = cstmt_empty();
    return (CStmt){
        .type = CSTMT_IF,
        .if_ = {
            .expr = expr,
            .stmt_true = alloc_move(alloc, stmt_true),
            .stmt_false = alloc_move(alloc, stmt_false),
        },
    };
}

static inline CStmt cstmt_if_else(CExpr expr, CStmt stmt_true, CStmt stmt_false, Alloc *alloc)
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

static inline CStmt cstmt_for(CStmt init_stmt, CExpr cond_expr, CExpr iter_expr, CStmt stmt, Alloc *alloc)
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

static inline CStmt cstmt_while(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_WHILE,
        .while_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

static inline CStmt cstmt_do(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_DO,
        .do_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

static inline CStmt cstmt_switch(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_SWITCH,
        .switch_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

static inline CStmt cstmt_return(CExpr expr)
{
    return (CStmt){
        .type = CSTMT_RETURN,
        .return_ = {
            .expr = expr,
        },
    };
}

static inline CStmt cstmt_goto(Str str, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_GOTO,
        .goto_.label = str_dup(str, alloc),
    };
}

static inline CStmt cstmt_case(CExpr expr)
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

static inline CType ctype_ptr(CType subtype, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_PTR,
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

static inline void ctype_constant_no_value(CType *self, Str name, Alloc *alloc)
{
    CTypeConstant constant = {
        .name = str_dup(name, alloc),
        .value = {.type = CVAL_INVALID}};

    if (self->type == CTYPE_ENUM)
    {
        vec_push(&self->enum_.constants, constant);
    }
    else
    {
        panic$("Only enums can have constants!");
    }
}

static inline CType ctype_func(CType ret, Str name, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_FUNC,
        .name = str_dup(name, alloc),
    };

    type.func_.ret = alloc_move(alloc, ret);

    vec_init(&type.func_.params, alloc);
    return type;
}

static inline CType ctype_name(Str name, Alloc *alloc)
{
    CType type =
        {
            .type = CTYPE_NAME,
            .name = str_dup(name, alloc),
        };

    return type;
}

static inline CType ctype_attr(CType type, CTypeAttr attr)
{
    type.attr |= attr;
    return type;
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

/* --- CUnit ---------------------------------------------------------------- */

static inline CUnit cunit(Alloc *alloc)
{
    CUnit unit;
    vec_init(&unit.units, alloc);
    return unit;
}

static inline void cunit_member(CUnit *unit, CUnitEntry entry)
{
    vec_push(&unit->units, entry);
}

static inline CUnitEntry cunit_decl(CDecl decl)
{
    return (CUnitEntry){
        .type = CUNIT_DECLARATION,
        ._decl = decl,
    };
}

static inline CUnitEntry cunit_pragma(Alloc *alloc)
{
    CPragma pragma;
    vec_init(&pragma.args, alloc);

    return (CUnitEntry){
        .type = CUNIT_PRAGMA,
        ._pragma = pragma,
    };
}

static inline CUnitEntry cunit_pragma_once(Alloc *alloc)
{
    CUnitEntry entry = cunit_pragma(alloc);
    vec_push(&entry._pragma.args, str_dup(str$("once"), alloc));
    return entry;
}

static inline CUnitEntry cunit_include(bool qchar, Str path, Alloc *alloc)
{
    return (CUnitEntry){
        .type = CUNIT_INCLUDE,
        ._include = (CInclude){
            .is_q_char = qchar,
            .path = str_dup(path, alloc)}};
}
