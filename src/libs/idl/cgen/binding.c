#include <brutal-fmt>
#include <cc/builder.h>
#include <idl/cgen.h>

CExpr idl_cgen_binding(IdlMethod method, IdlIface const iface, Alloc *alloc)
{
    CExpr init = cexpr_initializer(ctype_ident(str$("IdlBinding")), alloc);

    cexpr_member(&init, cexpr_constant(cval_unsigned(iface.id)));

    CExpr nullptr_ident = cexpr_ident(str$("nullptr"));

    cexpr_member(&init, cexpr_ident(fmt_str$(alloc, "MSG_{case-constant}_REQ", method.name)));

    if (method.request.type != IDL_TYPE_NIL)
    {
        cexpr_member(&init, idl_cgen_pack_ref(*method.request.primitive_.alias, alloc));
        cexpr_member(&init, idl_cgen_unpack_ref(*method.request.primitive_.alias, alloc));
    }
    else
    {
        cexpr_member(&init, nullptr_ident);
        cexpr_member(&init, nullptr_ident);
    }

    if (method.response.type != IDL_TYPE_NIL)
    {
        cexpr_member(&init, cexpr_ident(fmt_str$(alloc, "MSG_{case-constant}_RESP", method.name)));
        cexpr_member(&init, idl_cgen_pack_ref(*method.response.primitive_.alias, alloc));
        cexpr_member(&init, idl_cgen_unpack_ref(*method.response.primitive_.alias, alloc));
    }
    else
    {
        cexpr_member(&init, cexpr_constant(cval$(-1)));
        cexpr_member(&init, nullptr_ident);
        cexpr_member(&init, nullptr_ident);
    }

    return init;
}
