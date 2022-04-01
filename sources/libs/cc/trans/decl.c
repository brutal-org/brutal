#include <cc/trans.h>

static void cc_trans_decl_attr(Emit *emit, CDeclAttr attr)
{
    if (attr & CDECL_AUTO)
    {
        emit_fmt$(emit, "auto ");
    }

    if (attr & CDECL_STATIC)
    {
        emit_fmt$(emit, "static ");
    }

    if (attr & CDECL_REGISTER)
    {
        emit_fmt$(emit, "register ");
    }

    if (attr & CDECL_INLINE)
    {
        emit_fmt$(emit, "inline ");
    }

    if (attr & CDECL_EXTERN)
    {
        emit_fmt$(emit, "extern ");
    }

    if (attr & CDECL_THREAD)
    {
        emit_fmt$(emit, "thread ");
    }
}

void cc_trans_decl(Emit *emit, CDecl decl)
{
    cc_trans_decl_attr(emit, decl.attr);

    if (decl.type == CDECL_TYPE)
    {
        bool is_typedef = decl.name.len != 0;

        if (is_typedef)
        {
            emit_fmt$(emit, "typedef ");
        }

        cc_trans_type_start(emit, decl.type_.type);

        if (is_typedef)
        {
            emit_fmt$(emit, " {}", decl.name);
        }

        cc_trans_type_end(emit, decl.type_.type);
    }
    else if (decl.type == CDECL_VAR)
    {
        cc_trans_type_start(emit, decl.var_.type);
        emit_fmt$(emit, " {} ", decl.name);
        cc_trans_type_end(emit, decl.var_.type);

        if (decl.var_.expr.type != CEXPR_INVALID &&
            decl.var_.expr.type != CEXPR_EMPTY)
        {
            emit_fmt$(emit, "=");
            cc_trans_expr(emit, decl.var_.expr);
        }
    }
    else if (decl.type == CDECL_FUNC)
    {
        CType func_type = decl.func_.type;

        // Declarator
        cc_trans_type_start(emit, func_type);
        emit_fmt$(emit, " {}", decl.name);
        cc_trans_type_end(emit, func_type);

        // Body
        if (decl.func_.body.type != CSTMT_EMPTY)
        {
            emit_fmt$(emit, "\n", decl.name);
            cc_trans_stmt(emit, decl.func_.body);
        }
        else
        {
            emit_fmt$(emit, ";", decl.name);
        }
    }
}
