#include <cc/builder.h>
#include <cc/sema/visit.h>

typedef Vec(CType) CTypeReturned;

CType csema_expr_return_type(CSema *sema, CExpr expr, Alloc *alloc)
{
}
bool csema_stmt_return_type(CSema *sema, CStmt stmt, Alloc *alloc, CTypeReturned *result)
{
    switch (stmt.type)
    {
    case CSTMT_RETURN:
    {
        vec_push(result, csema_expr_return_type(sema, stmt.return_.expr, alloc));
		return true;
    }
	case CSTMT_BLOCK:
	{
		vec_foreach(sub_stmt, &stmt.block_.stmts)
		{
			if(!csema_stmt_return_type(sema, *sub_stmt, alloc, result))
			{
				return false;
			}
		}
		break;
	}
	case CSTMT_
    }
}
/* for the moment statement don't have type maybe later */
CStmt csema_stmt(CSema *sema, CStmt stmt, Alloc *alloc)
{
    switch (stmt.type)
    {
    case CSTMT_DECL:
    {
        *stmt.decl_.decl = csema_decl(sema, *stmt.decl_.decl, alloc);
        stmt.sema_type = stmt.decl_.decl->sema_type;
        break;
    }
    case CSTMT_EXPR:
    {
        stmt.expr_.expr = csema_expr(sema, stmt.expr_.expr, alloc);
        stmt.sema_type = stmt.expr_.expr.sema_type;
        break;
    }
    /* this is not good for compound statement but for the moment it works */
    case CSTMT_BLOCK:
    {
        CType type = ctype_void();
        vec_foreach(sub_stmt, &stmt.block_.stmts)
        {
            *sub_stmt = csema_stmt(sema, *sub_stmt, alloc);
        }
        break;
    }
    case CSTMT_IF:
    {
        stmt.if_.expr = csema_expr(sema, stmt.if_.expr, alloc);
        break;
    }
    }
}
