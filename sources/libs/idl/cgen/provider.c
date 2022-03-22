#include <brutal/text.h>
#include <cc/builder.h>
#include <idl/cgen.h>

CType idl_cgen_provider_type(IdlIface const iface, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_ident(str$("IpcCap")), alloc);
    CType vtable_type = ctype_ident(str_fmt$(alloc, "{}VTable", iface.name));
    CType ipc_type = ctype_ident(str$("IpcComponent"));

    ctype_member(&ctype, str$("ipc"), ctype_ptr(ipc_type, alloc));
    ctype_member(&ctype, str$("vtable"), ctype_ptr(vtable_type, alloc));
    ctype_member(&ctype, str$("ctx"), ctype_ptr(ctype_void(), alloc));

    return ctype;
}

CDecl idl_cgen_provider_func(IdlIface const iface, Alloc *alloc)
{
    Str name = str_fmt$(alloc, "{case:snake}_provide", iface.name);
    CType type = idl_cgen_provider_type(iface, alloc);

    CStmt body = cstmt_block(alloc);
    CExpr expr = cexpr_call(alloc, cexpr_ident(str$("ipc_component_provide")));

    cexpr_member(&expr, cexpr_ident(str$("ipc")));
    cexpr_member(&expr, cexpr_constant(cval_unsigned(iface.id)));
    cexpr_member(&expr, cexpr_ident(str_fmt$(alloc, "__IDL_PRIVATE__{case:snake}_dispatch_rpc", iface.name)));
    cexpr_member(&expr, cexpr_ident(str$("vtable")));
    cexpr_member(&expr, cexpr_ident(str$("ctx")));

    cstmt_block_add(&body, cstmt_return(expr));

    return cdecl_func(name, type, body);
}
