#include <cc/sema/check/decl.h>
#include <cc/sema/check/utils.h>

// error checking with declaration redefinition
void sema_decl_redef_check(CSema *self, CDecl *decl, MAYBE_UNUSED void *ctx)
{
    CDecl other = csema_lookup(self, decl->name);
    bool redefinition = false;
    if (decl->type == CDECL_VAR)
    {
        // var + var = error
        if (other.type == CDECL_VAR)
        {
            redefinition = true;
        }
        // global var + type / func = error
        else if ((other.type == CDECL_TYPE || other.type == CDECL_FUNC) && decl->global == true)
        {
            redefinition = true;
        }
    }
    else
    {
        // func / type + global var = error
        if (other.type == CDECL_VAR && other.global)
        {
            redefinition = true;
        }
        // func / type + func / type = error
        else if (other.type == CDECL_TYPE || other.type == CDECL_FUNC)
        {
            redefinition = true;
        }
    }

    if (redefinition)
    {
        int id = csema_report$(self, PARSE_ERR, *decl, "redefinition of: {}", decl->name);
        csema_comment$(self, id, other, "already defined here");
    }
}

void sema_decl_ref_check_expr(CSema *self, CExpr *expr, MAYBE_UNUSED void *ctx)
{
    if (expr->type != CEXPR_IDENT)
    {
        return;
    }

    CDecl prev = csema_lookup(self, expr->ident_);

    if (prev.type == CDECL_NIL)
    {
        csema_report$(self, PARSE_ERR, *expr, "Not defined symbol: {}", expr->ident_);
    }
}
