#include <brutal-fmt>
#include <cc/builder.h>
#include <idl/cgen.h>

CType idl_cgen_provider_type(IdlIface const iface, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_ident(str$("IpcCap")), alloc);
    CType vtable_type = ctype_ident(fmt_str$(alloc, "{}VTable", iface.name));
    CType ipc_type = ctype_ident(str$("IpcObject"));

    ctype_member(&ctype, str$("object"), ctype_ptr(ipc_type, alloc));
    ctype_member(&ctype, str$("vtable"), ctype_ptr(vtable_type, alloc));

    return ctype;
}

CDecl idl_cgen_provider_func(IdlIface const iface, Alloc *alloc)
{
    Str name = fmt_str$(alloc, "{case-snake}_provide", iface.name);
    CType type = idl_cgen_provider_type(iface, alloc);

    CStmt body = cstmt_block(alloc);
    CExpr expr = cexpr_call(alloc, cexpr_ident(str$("ipc_object_provide")));

    cexpr_member(&expr, cexpr_ident(str$("object")));
    cexpr_member(&expr, cexpr_constant(cval_unsigned(iface.id)));
    cexpr_member(&expr, cexpr_ident(fmt_str$(alloc, "__IDL_PRIVATE__{case-snake}_dispatch_rpc", iface.name)));
    cexpr_member(&expr, cexpr_ident(str$("vtable")));

    cstmt_block_add(&body, cstmt_return(expr));

    return cdecl_func(name, type, body);
}
