#include <cc/builder.h>
#include <cc/sema/visit.h>

typedef Vec(CType) CTypeReturned;

/* for the moment statement don't have type maybe later */
CStmt csema_stmt(CSema *sema, CStmt stmt, Alloc *alloc)
{
    switch (stmt.type)
    {
    case CSTMT_DECL:
    {
        *stmt.decl_.decl = csema_decl(sema, *stmt.decl_.decl, alloc);
        break;
    }
    case CSTMT_EXPR:
    {
        stmt.expr_.expr = csema_expr(sema, stmt.expr_.expr, alloc);
        break;
    }
    case CSTMT_BLOCK:
    {
        csema_scope_enter(sema);
        vec_foreach(sub_stmt, &stmt.block_.stmts)
        {
            *sub_stmt = csema_stmt(sema, *sub_stmt, alloc);
        }
        csema_scope_leave(sema);
        break;
    }
    case CSTMT_IF:
    {
        stmt.if_.expr = csema_expr(sema, stmt.if_.expr, alloc);
        break;
    }
    case CSTMT_FOR:
    {
        *stmt.for_.init_stmt = csema_stmt(sema, *stmt.for_.init_stmt, alloc);
        stmt.for_.cond_expr = csema_expr(sema, stmt.for_.cond_expr, alloc);
        stmt.for_.iter_expr = csema_expr(sema, stmt.for_.iter_expr, alloc);
        *stmt.for_.stmt = csema_stmt(sema, *stmt.for_.stmt, alloc);
        break;
    }
    case CSTMT_WHILE:
    case CSTMT_DO:
    case CSTMT_SWITCH:
    {
        *stmt.switch_.stmt = csema_stmt(sema, *stmt.switch_.stmt, alloc);
        stmt.switch_.expr = csema_expr(sema, stmt.switch_.expr, alloc);
        break;
    }
    case CSTMT_BREAK:
    case CSTMT_CONTINUE:
    case CSTMT_LABEL:
    case CSTMT_DEFAULT:
    case CSTMT_GOTO:
    {
        break;
    }
    case CSTMT_CASE:
    {
        stmt.case_.expr = csema_expr(sema, stmt.case_.expr, alloc);
        break;
    }
    case CSTMT_RETURN:
    {
        stmt.return_.expr = csema_expr(sema, stmt.return_.expr, alloc);
        break;
    }
    default:
    {
        break;
    }
    }
    return stmt;
}
