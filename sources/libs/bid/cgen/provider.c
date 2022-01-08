#include <bid/cgen.h>
#include <brutal/io.h>
#include <cc/builder.h>

CType bid_cgen_provider_type(BidIface const iface, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);
    CType vtable_type = ctype_ident(str_fmt(alloc, "{}VTable", iface.name));
    CType ipc_type = ctype_ident(str$("IpcEv"));

    ctype_member(&ctype, str$("ev"), ctype_ptr(ipc_type, alloc));
    ctype_member(&ctype, str$("vtable"), ctype_ptr(vtable_type, alloc));
    return ctype;
}

CDecl bid_cgen_provider_func(BidIface const iface, Alloc *alloc)
{
    Str name = str_fmt(alloc, "{case:lower}_provide", iface.name);
    CType type = bid_cgen_provider_type(iface, alloc);

    CStmt body = cstmt_block(alloc);
    CExpr expr = cexpr_call(alloc, cexpr_ident(str$("br_ev_provide")));

    // void br_ev_provide(IpcEv *self, uint32_t id, IpcFn *fn, void *ctx);

    cexpr_member(&expr, cexpr_ident(str$("ev")));
    cexpr_member(&expr, cexpr_constant(cval_unsigned(iface.id)));
    cexpr_member(&expr, cexpr_ident(str_fmt(alloc, "{case:snake}_dispatch", iface.name)));
    cexpr_member(&expr, cexpr_ident(str$("vtable")));

    cstmt_block_add(&body, cstmt_expr(expr));

    return cdecl_func(name, type, body);
}
