#include <brutal/io.h>
#include <cc/builder.h>
#include <idl/cgen.h>

CExpr idl_cgen_binding(IdlMethod method, IdlIface const iface, Alloc *alloc)
{
    CExpr init = cexpr_initializer(alloc);

    cexpr_member(&init, cexpr_constant(cval_signed(iface.id)));

    CExpr nullptr_ident = cexpr_ident(str$("nullptr"));

    if (method.request.type != IDL_TYPE_NIL)
    {
        cexpr_member(&init, cexpr_ident(str_fmt(alloc, "MSG_{case:constant}_REQ", method.mangled)));
        cexpr_member(&init, idl_cgen_pack_ref(method.request.primitive_.mangled, alloc));
        cexpr_member(&init, idl_cgen_unpack_ref(method.request.primitive_.mangled, alloc));
    }
    else
    {
        cexpr_member(&init, cexpr_constant(cval$(-1)));
        cexpr_member(&init, nullptr_ident);
        cexpr_member(&init, nullptr_ident);
    }

    if (method.response.type != IDL_TYPE_NIL)
    {
        cexpr_member(&init, cexpr_ident(str_fmt(alloc, "MSG_{case:constant}_RESP", method.mangled)));
        cexpr_member(&init, idl_cgen_pack_ref(method.response.primitive_.mangled, alloc));
        cexpr_member(&init, idl_cgen_unpack_ref(method.response.primitive_.mangled, alloc));
    }
    else
    {
        cexpr_member(&init, cexpr_constant(cval$(-1)));
        cexpr_member(&init, nullptr_ident);
        cexpr_member(&init, nullptr_ident);
    }

    return cexpr_cast(init, ctype_ident(str$("IdlBinding")), alloc);
}
