#include <brutal-debug>
#include <cc/builder.h>
#include <cc/dump.h>
#include <cc/sema/visit.h>

bool cvisit_node_is(void *node, int what, int type)
{
    if (what & CVISIT_DECL)
    {
        return (int)((CDecl *)node)->type == type;
    }
    else if (what & CVISIT_EXPR)
    {
        return (int)((CExpr *)node)->type == type;
    }
    else if (what & CVISIT_STMT)
    {
        return (int)((CStmt *)node)->type == type;
    }
    else if (what & CVISIT_TYPE)
    {
        return (int)((CType *)node)->type == type;
    }
    else if (what & CVISIT_UNIT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void cvisit_before(CVisites passes, CSema *sema, int what, void *node)
{
    slice_foreach$(pass, passes)
    {
        int mask = what | CVISIT_BEGIN;
        if ((pass->what & mask) == mask)
        {
            pass->fn(sema, what, node);
        }
    }
}

void cvisit_after(CVisites passes, CSema *sema, int what, void *node)
{
    slice_foreach_rev$(pass, passes)
    {
        int mask = what | CVISIT_END;
        if ((pass->what & mask) == mask)
        {
            pass->fn(sema, what, node);
        }
    }
}

void cvisit_decl(CVisites passes, CSema *sema, CDecl *decl)
{
    cvisit_before(passes, sema, CVISIT_DECL, decl);

    switch (decl->type)
    {
    case CDECL_EMPTY:
        break;

    case CDECL_TYPE:
        cvisit_type(passes, sema, &decl->type_.type);
        break;

    case CDECL_VAR:
        cvisit_type(passes, sema, &decl->var_.type);
        cvisit_expr(passes, sema, &decl->var_.expr);
        break;

    case CDECL_FUNC:
        cvisit_type(passes, sema, &decl->func_.type);
        cvisit_stmt(passes, sema, &decl->func_.body);
        break;

    default:
        panic$("Unknow decl type");
    }

    cvisit_after(passes, sema, CVISIT_DECL, decl);
}

void cvisit_expr(CVisites passes, CSema *sema, CExpr *expr)
{
    cvisit_before(passes, sema, CVISIT_EXPR, expr);

    switch (expr->type)
    {
    case CEXPR_IDENT:
    case CEXPR_CONSTANT:
    case CEXPR_EMPTY:
        break;

    case CEXPR_POSTFIX:
    case CEXPR_PREFIX:
        cvisit_expr(passes, sema, expr->postfix_.expr);
        break;

    case CEXPR_INFIX:
        cvisit_expr(passes, sema, expr->infix_.lhs);
        cvisit_expr(passes, sema, expr->infix_.rhs);
        break;

    case CEXPR_CALL:
        cvisit_expr(passes, sema, expr->call_.expr);
        vec_foreach(arg, &expr->call_.args)
        {
            cvisit_expr(passes, sema, arg);
        }
        break;

    case CEXPR_CAST:
        cvisit_type(passes, sema, &expr->cast_.type);
        cvisit_expr(passes, sema, expr->cast_.expr);
        break;

    case CEXPR_TERNARY:
        cvisit_expr(passes, sema, expr->ternary_.expr_cond);
        cvisit_expr(passes, sema, expr->ternary_.expr_true);
        cvisit_expr(passes, sema, expr->ternary_.expr_false);
        break;

    case CEXPR_INITIALIZER:
        cvisit_type(passes, sema, &expr->initializer_.type);

        vec_foreach(init, &expr->initializer_.initializer)
        {
            cvisit_expr(passes, sema, init);
        }
        break;

    case CEXPR_LAMBDA:
        cvisit_type(passes, sema, &expr->lambda_.type);
        cvisit_stmt(passes, sema, expr->lambda_.body);
        break;

    default:
        panic$("Unknow expr type");
        break;
    }

    cvisit_after(passes, sema, CVISIT_EXPR, expr);
}

void cvisit_stmt(CVisites passes, CSema *sema, CStmt *stmt)
{
    cvisit_before(passes, sema, CVISIT_STMT, stmt);

    switch (stmt->type)
    {
    case CSTMT_DECL:
        cvisit_decl(passes, sema, stmt->decl_.decl);
        break;

    case CSTMT_EXPR:
        cvisit_expr(passes, sema, &stmt->expr_.expr);
        break;

    case CSTMT_BLOCK:
        vec_foreach(v, &stmt->block_.stmts)
        {
            cvisit_stmt(passes, sema, v);
        }
        break;

    case CSTMT_IF:
        cvisit_expr(passes, sema, &stmt->if_.expr);
        cvisit_stmt(passes, sema, stmt->if_.stmt_true);
        cvisit_stmt(passes, sema, stmt->if_.stmt_false);
        break;

    case CSTMT_FOR:
        cvisit_stmt(passes, sema, stmt->for_.init_stmt);
        cvisit_expr(passes, sema, &stmt->for_.cond_expr);
        cvisit_expr(passes, sema, &stmt->for_.iter_expr);

        cvisit_stmt(passes, sema, stmt->for_.stmt);
        break;

    case CSTMT_WHILE:
    case CSTMT_DO:
    case CSTMT_SWITCH:
        cvisit_expr(passes, sema, &stmt->while_.expr);
        cvisit_stmt(passes, sema, stmt->while_.stmt);
        break;

    case CSTMT_RETURN:
        cvisit_expr(passes, sema, &stmt->return_.expr);
        break;

    case CSTMT_CASE:
        cvisit_expr(passes, sema, &stmt->case_.expr);
        break;

    default:
        panic$("Unknow stmt type");
        break;
    }

    cvisit_after(passes, sema, CVISIT_STMT, stmt);
}

void cvisit_type(CVisites passes, CSema *sema, CType *type)
{
    cvisit_before(passes, sema, CVISIT_TYPE, type);

    switch (type->type)
    {
    case CTYPE_ARRAY:
    {
        cvisit_type(passes, sema, type->array_.subtype);
        break;
    }

    case CTYPE_PTR:
    case CTYPE_REF:
    case CTYPE_PARENT:
        cvisit_type(passes, sema, type->ref_.subtype);
        break;

    case CTYPE_STRUCT:
    case CTYPE_UNION:
        vec_foreach(member, &type->struct_.members)
        {
            cvisit_type(passes, sema, &member->type);
        }
        break;

    case CTYPE_FUNC:
        cvisit_type(passes, sema, type->func_.ret);

        vec_foreach(func_arg, &type->func_.params)
        {
            cvisit_type(passes, sema, &func_arg->type);
        }
        break;

    default:
        panic$("Unknow type type");
        break;
    }

    cvisit_after(passes, sema, CVISIT_TYPE, type);
}

void cvisit_unit(CVisites passes, CSema *sema, CUnit *unit)
{
    cvisit_before(passes, sema, CVISIT_UNIT, unit);

    vec_foreach(entry, &unit->units)
    {
        if (entry->type == CUNIT_DECLARATION)
        {
            cvisit_decl(passes, sema, &entry->_decl);
        }
    }

    cvisit_after(passes, sema, CVISIT_UNIT, unit);
}
