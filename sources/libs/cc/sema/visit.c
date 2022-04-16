#include <cc/builder.h>
#include <cc/sema/visit.h>
#include <brutal-debug>

CType csema_value_type(MAYBE_UNUSED CSema *sema, CVal value, Alloc *alloc)
{
    /* FIXME: autodetect precision based on architecture */
    switch (value.type)
    {
    case CVAL_FLOAT:
        return ctype_float(32);
    case CVAL_SIGNED:
        return ctype_signed(32);
    case CVAL_UNSIGNED:
        return ctype_unsigned(32);
    case CVAL_STRING:
        return ctype_array(ctype_signed(8), value.string_.len, alloc);
    default:
        return ctype_void(); /* FIXME: better error handling */
    }
}

static CType csema_decl_type(CSema *self, Str name)
{
    vec_foreach(scopes, &self->scopes)
    {
        vec_foreach(decls, &scopes->decls)
        {
            if (str_eq(decls->name, name))
            {
                return decls->sema_type;
            }
        }
    }
    log$("not founded symbol: {}", name);
    return ctype_void();
}

static CExpr csema_prefix_expr(CSema *sema, CExpr expr, Alloc *alloc)
{
    *expr.prefix_.expr = csema_expr(sema, *expr.prefix_.expr, alloc);
    switch (expr.prefix_.op)
    {
    case COP_REF:
    {
        expr.sema_type = ctype_ptr(expr.prefix_.expr->sema_type, alloc);
        return expr;
    }
    case COP_DEREF:
    {
        if (!(expr.prefix_.expr->sema_type.type == CTYPE_PTR))
        {
            log$("trying to deref a ptr");
            return expr;
        }
        expr.sema_type = *expr.prefix_.expr->sema_type.ptr_.subtype;
        return expr;
    }

    case COP_NOT:
    case COP_DEC:
    case COP_INC:
    {
        expr.sema_type = expr.prefix_.expr->sema_type;
        return expr;
    }
    default:
    {
        return expr;
    }
    }
}
static CExpr csema_postfix_expr(CSema *sema, CExpr expr, Alloc *alloc)
{
    *expr.postfix_.expr = csema_expr(sema, *expr.postfix_.expr, alloc);
    switch (expr.postfix_.op)
    {
    case COP_DEC:
    case COP_INC:
    {
        expr.sema_type = expr.postfix_.expr->sema_type;
        return expr;
    }
    default:
    {
        return expr;
    }
    }
}

static CExpr csema_infix_expr(CSema *sema, CExpr expr, Alloc *alloc)
{
    *expr.infix_.lhs = csema_expr(sema, *expr.infix_.lhs, alloc);
    *expr.infix_.rhs = csema_expr(sema, *expr.infix_.rhs, alloc);

    switch (expr.infix_.op)
    {
    case COP_INDEX:
    {

        if (expr.infix_.lhs->sema_type.type == CTYPE_ARRAY)
        {
            expr.sema_type = *expr.infix_.lhs->sema_type.array_.subtype;
        }
        else if (expr.infix_.lhs->sema_type.type == CTYPE_PTR)
        {
            expr.sema_type = *expr.infix_.lhs->sema_type.ptr_.subtype;
        }
        else
        {
            log$("trying to index a non array");
            expr.sema_type = ctype_void();
        }
        return expr;
    }
    /* for every unsupported basic op like add sub etc... this works */
    default:
    {
        expr.sema_type = expr.infix_.lhs->sema_type;
        return expr;
    }
    }
}

CExpr csema_expr(CSema *sema, CExpr expr, Alloc *alloc)
{
    switch (expr.type)
    {
    case CEXPR_CONSTANT:
    {
        expr.sema_type = csema_value_type(sema, expr.constant_, alloc);
        return expr;
    }
    case CEXPR_IDENT:
    {
        expr.sema_type = csema_decl_type(sema, expr.ident_);
        return expr;
    }
    case CEXPR_CAST:
    {
        /* don't check for the moment */
        expr.sema_type = expr.cast_.type;
        return expr;
    }
    case CEXPR_CALL:
    {
        *expr.call_.expr = csema_expr(sema, *expr.call_.expr, alloc);

        if (expr.call_.expr->sema_type.type != CTYPE_FUNC)
        {
            log$("not a function! ");
            return expr;
        }
        /* FIXME: no check for this for the moment */
        vec_foreach(arg, &expr.call_.args)
        {
            *arg = csema_expr(sema, *arg, alloc);
        }
        expr.sema_type = *expr.call_.expr->sema_type.func_.ret;
        return expr;
    }
    case CEXPR_LAMBDA:
    {
        expr.sema_type = ctype_void();
        return expr;
    }
    case CEXPR_PREFIX:
    {
        return csema_prefix_expr(sema, expr, alloc);
    }
    case CEXPR_INFIX:
    {
        return csema_infix_expr(sema, expr, alloc);
    }
    case CEXPR_POSTFIX:
    {
        return csema_postfix_expr(sema, expr, alloc);
    }
    default:
    {
        log$("unhandled expr type: {}", expr.type);
        expr.sema_type = ctype_void();
        return expr;
    }
    }
}

CDecl csema_decl(CSema *sema, CDecl decl, Alloc *alloc)
{
    if (csema_lookup(sema, decl.name).type != CDECL_NIL)
    {
        log$("redefinition of: {}", decl.name);
        return cdecl_empty();
    }

    switch (decl.type)
    {
    case CDECL_TYPE:
    {
        decl.sema_type = decl.type_.type;
        break;
    }
    case CDECL_FUNC:
    {
        decl.sema_type = decl.func_.type;
        csema_scope_add(sema, decl); /* we need to add the function to the scope before the body lookup because the body can call the function it self */

        csema_scope_enter_func(sema, decl.func_.type);
        decl.func_.body = csema_stmt(sema, decl.func_.body, alloc);
        csema_scope_leave(sema);
        break;
    }
    case CDECL_VAR:
    {
        decl.sema_type = decl.var_.type;
        decl.var_.expr = csema_expr(sema, decl.var_.expr, alloc);
        csema_scope_add(sema, decl);
        break;
    }
    default:
    {
        decl.sema_type = ctype_void();
        break;
    }
    }

    return decl;
}

CUnit csema_unit(CSema *sema, CUnit unit, Alloc *alloc)
{
    CUnit result = cunit(alloc);

    csema_scope_enter(sema);

    vec_foreach(entry, &unit.units)
    {
        if (entry->type == CUNIT_DECLARATION)
        {
            cunit_decl(&result, csema_decl(sema, entry->_decl, alloc));
        }
    }

    csema_scope_leave(sema);

    return result;
}
