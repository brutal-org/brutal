#include <brutal/io.h>
#include <cc/builder.h>
#include <idl/cgen.h>

CType idl_cgen_provider_type(IdlIface const iface, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_ident(str$("IpcCapability")), alloc);
    CType vtable_type = ctype_ident(str_fmt(alloc, "{}VTable", iface.name));
    CType ipc_type = ctype_ident(str$("IpcComponent"));

    ctype_member(&ctype, str$("ev"), ctype_ptr(ipc_type, alloc));
    ctype_member(&ctype, str$("vtable"), ctype_ptr(vtable_type, alloc));
    return ctype;
}

CDecl idl_cgen_provider_func(IdlIface const iface, Alloc *alloc)
{
    Str name = str_fmt(alloc, "{case:lower}_provide", iface.name);
    CType type = idl_cgen_provider_type(iface, alloc);

    CStmt body = cstmt_block(alloc);
    CExpr expr = cexpr_call(alloc, cexpr_ident(str$("ipc_component_provide")));

    cexpr_member(&expr, cexpr_ident(str$("ev")));
    cexpr_member(&expr, cexpr_constant(cval_unsigned(iface.id)));
    cexpr_member(&expr, cexpr_ident(str_fmt(alloc, "{case:snake}_dispatch", iface.name)));
    cexpr_member(&expr, cexpr_ident(str$("vtable")));

    cstmt_block_add(&body, cstmt_return(expr));

    return cdecl_func(name, type, body);
}
