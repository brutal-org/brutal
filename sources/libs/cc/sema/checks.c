#include <brutal/debug.h>
#include <cc/sema/checks.h>

// see checks.h header for more explanation

void do_sema_check_decl(SemaCheck *check, CDecl *decl, CSema *sema);
void do_sema_check_stmt(SemaCheck *check, CStmt *stmt, CSema *sema);

void do_sema_check_type(SemaCheck *check, CType *type, CSema *sema)
{
    if (check->type == CSEMA_CHECK_TYPE)
    {
        check->_type(sema, type, check->ctx);
    }

    switch (type->type)
    {
    case CTYPE_ARRAY:
    {
        do_sema_check_type(check, type->array_.subtype, sema);
        break;
    }

    case CTYPE_PTR:
    case CTYPE_REF:
    case CTYPE_PARENT:
    {
        do_sema_check_type(check, type->ref_.subtype, sema);
        break;
    }
    case CTYPE_STRUCT:
    case CTYPE_UNION:
    {
        vec_foreach(member, &type->struct_.members)
        {
            do_sema_check_type(check, &member->type, sema);
        }
        break;
    }
    case CTYPE_FUNC:
    {
        do_sema_check_type(check, type->func_.ret, sema);
        vec_foreach(func_arg, &type->func_.params)
        {
            do_sema_check_type(check, &func_arg->type, sema);
        }

        break;
    }
    default:
    {
        break;
    }
    }
}

void do_sema_check_expr(SemaCheck *check, CExpr *expr, CSema *sema)
{
    if (check->type == CSEMA_CHECK_EXPR)
    {
        check->_expr(sema, expr, check->ctx);
    }

    switch (expr->type)
    {

    case CEXPR_IDENT:
    case CEXPR_CONSTANT:
    case CEXPR_EMPTY:
    {
        break;
    }
    case CEXPR_POSTFIX:
    case CEXPR_PREFIX:
    {
        do_sema_check_expr(check, expr->postfix_.expr, sema);
        break;
    }
    case CEXPR_INFIX:
    {
        do_sema_check_expr(check, expr->infix_.lhs, sema);
        do_sema_check_expr(check, expr->infix_.rhs, sema);
        break;
    }
    case CEXPR_CALL:
    {
        do_sema_check_expr(check, expr->call_.expr, sema);
        vec_foreach(arg, &expr->call_.args)
        {
            do_sema_check_expr(check, arg, sema);
        }
        break;
    }
    case CEXPR_CAST:
    {
        do_sema_check_type(check, &expr->cast_.type, sema);
        do_sema_check_expr(check, expr->cast_.expr, sema);
        break;
    }
    case CEXPR_TERNARY:
    {
        do_sema_check_expr(check, expr->ternary_.expr_cond, sema);
        do_sema_check_expr(check, expr->ternary_.expr_true, sema);
        do_sema_check_expr(check, expr->ternary_.expr_false, sema);
        break;
    }
    case CEXPR_INITIALIZER:
    {
        do_sema_check_type(check, &expr->initializer_.type, sema);
        vec_foreach(init, &expr->initializer_.initializer)
        {
            do_sema_check_expr(check, init, sema);
        }
        break;
    }
    case CEXPR_LAMBDA:
    {
        do_sema_check_type(check, &expr->lambda_.type, sema);
        do_sema_check_stmt(check, expr->lambda_.body, sema);
        break;
    }
    default:
    {
        panic$("unhandled expr: {}", expr->type);
    }
    }
}

void do_sema_check_stmt(SemaCheck *check, CStmt *stmt, CSema *sema)
{

    if (check->type == CSEMA_CHECK_STMT)
    {
        check->_stmt(sema, stmt, check->ctx);
    }

    switch (stmt->type)
    {
    case CSTMT_DECL:
    {
        do_sema_check_decl(check, stmt->decl_.decl, sema);
        break;
    }
    case CSTMT_EXPR:
    {
        do_sema_check_expr(check, &stmt->expr_.expr, sema);
        break;
    }
    case CSTMT_BLOCK:
    {
        csema_scope_enter(sema);
        vec_foreach(v, &stmt->block_.stmts)
        {
            do_sema_check_stmt(check, v, sema);
        }
        csema_scope_leave(sema);
        break;
    }
    case CSTMT_IF:
    {
        do_sema_check_expr(check, &stmt->if_.expr, sema);
        do_sema_check_stmt(check, stmt->if_.stmt_true, sema);
        do_sema_check_stmt(check, stmt->if_.stmt_false, sema);
        break;
    }
    case CSTMT_FOR:
    {
        do_sema_check_stmt(check, stmt->for_.init_stmt, sema);
        do_sema_check_expr(check, &stmt->for_.cond_expr, sema);
        do_sema_check_expr(check, &stmt->for_.iter_expr, sema);

        do_sema_check_stmt(check, stmt->for_.stmt, sema);
        break;
    }
    case CSTMT_WHILE:
    case CSTMT_DO:
    case CSTMT_SWITCH:
    {
        do_sema_check_expr(check, &stmt->while_.expr, sema);
        do_sema_check_stmt(check, stmt->while_.stmt, sema);
        break;
    }
    case CSTMT_RETURN:
    {
        do_sema_check_expr(check, &stmt->return_.expr, sema);
        break;
    }
    case CSTMT_CASE:
    {
        do_sema_check_expr(check, &stmt->case_.expr, sema);
        break;
    }
    default:
        break;
    }
}

void do_sema_check_decl(SemaCheck *check, CDecl *decl, CSema *sema)
{
    if (check->type != CSEMA_CHECK_DECL || check->_decl.include_recursive_entry)
    {
        csema_scope_add(sema, *decl);
    }

    if (check->type == CSEMA_CHECK_DECL)
    {
        check->_decl.fn(sema, decl, check->ctx);
    }

    switch (decl->type)
    {
    case CDECL_TYPE:
        do_sema_check_type(check, &decl->type_.type, sema);
        break;
    case CDECL_VAR:
        do_sema_check_type(check, &decl->var_.type, sema);
        do_sema_check_expr(check, &decl->var_.expr, sema);
        break;
    case CDECL_FUNC:
    {
        csema_scope_enter_func(sema, decl->func_.type);
        do_sema_check_type(check, &decl->func_.type, sema);
        do_sema_check_stmt(check, &decl->func_.body, sema);
        csema_scope_leave(sema);
        break;
    }
    default:
        break;
    }
    if (check->type == CSEMA_CHECK_DECL && !check->_decl.include_recursive_entry)
    {
        csema_scope_add(sema, *decl);
    }
}

static void do_sema_check_unit(SemaCheck *check, CUnit *unit, CSema *sema)
{
    csema_scope_enter(sema);

    if (check->type == CSEMA_CHECK_UNIT)
    {
        check->_unit(sema, unit, check->ctx);
        csema_scope_leave(sema);
        return;
    }

    vec_foreach(entry, &unit->units)
    {
        if (entry->type == CUNIT_DECLARATION)
        {

            do_sema_check_decl(check, &entry->_decl, sema);
        }
    }

    csema_scope_leave(sema);
}

void do_sema_check(SemaCheck *check, CUnit *unit, CSema *sema)
{
    do_sema_check_unit(check, unit, sema);
}
