#include <cc/builder.h>
#include <cc/sema/visit.h>

CVal csema_value(CSema *sema, CVal value, Alloc *alloc)
{
}

CType csema_type(CSema *sema, CType type, Alloc *alloc)
{
}

CExpr csema_expr(CSema *sema, CExpr expr, Alloc *alloc)
{
}

CStmt csema_stmt(CSema *sema, CStmt stmt, Alloc *alloc)
{
}

CDecl csema_decl(CSema *sema, CDecl decl, Alloc *alloc)
{
}

CUnit csema_unit(CSema *sema, CUnit unit, Alloc *alloc)
{
    CUnit result = cunit(alloc);

    csema_scope_leave(sema);

    vec_foreach(entry, &unit.units)
    {
        if (entry.type == CUNIT_DECLARATION)
        {
            csema_scope_add(sema, entry._decl);
            cunit_decl(&result, csema_decl(sema, entry._decl));
        }
    }

    csema_scope_leave(sema);

    return result;
}
