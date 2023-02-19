#include <brutal-fmt>
#include <cc/builder.h>
#include <idl/cgen.h>

CExpr idl_cgen_dispatch_vtable(IdlIface const iface, Alloc *alloc)
{
    CType vtable_type = ctype_ident_ptr(fmt_str$(alloc, "{}VTable", iface.name), alloc);
    return cexpr_cast(cexpr_ident(str$("vtable")), vtable_type, alloc);
}

CType idl_cgen_dispatch_type(Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);

    ctype_member(&ctype, str$("ipc"), ctype_ident_ptr(str$("IpcComponent"), alloc));
    ctype_member(&ctype, str$("object"), ctype_ident_ptr(str$("IpcObject"), alloc));
    ctype_member(&ctype, str$("req"), ctype_ident_ptr(str$("BrMsg"), alloc));
    ctype_member(&ctype, str$("vtable"), ctype_ptr(ctype_void(), alloc));

    return ctype;
}

CExpr idl_cgen_dispatch_handler(IdlMethod method, IdlIface const iface, Alloc *alloc)
{
    return cexpr_cast(cexpr_ptr_access(idl_cgen_dispatch_vtable(iface, alloc), cexpr_ident(method.name), alloc), ctype_ident_ptr(str$("IdlHandlerFn"), alloc), alloc);
}

void idl_cgen_dispatch_case(CStmt *block, IdlMethod method, IdlIface const iface, Alloc *alloc)
{
    CExpr call_handler = cexpr_call(alloc, cexpr_ident(str$("ipc_hook_handle")));

    cexpr_member(&call_handler, cexpr_ident(str$("ipc")));
    cexpr_member(&call_handler, cexpr_ident(str$("object")));
    cexpr_member(&call_handler, idl_cgen_dispatch_handler(method, iface, alloc));
    cexpr_member(&call_handler, cexpr_ident(str$("req")));
    cexpr_member(&call_handler, idl_cgen_binding(method, iface, alloc));

    CExpr nullptr_ident = cexpr_ident(str$("nullptr"));

    if (method.request.type != IDL_TYPE_NIL)
    {
        cstmt_block_add(block, cstmt_decl(cdecl_var(str$("req_buf"), idl_cgen_decl_primitive(method.request), cexpr_empty()), alloc));
        cexpr_member(&call_handler, cexpr_ref(cexpr_ident(str$("req_buf")), alloc));
    }
    else
    {
        cexpr_member(&call_handler, nullptr_ident);
    }

    if (method.response.type != IDL_TYPE_NIL)
    {
        cstmt_block_add(block, cstmt_decl(cdecl_var(str$("resp_buf"), idl_cgen_decl_primitive(method.response), cexpr_empty()), alloc));
        cexpr_member(&call_handler, cexpr_ref(cexpr_ident(str$("resp_buf")), alloc));
    }
    else
    {
        cexpr_member(&call_handler, nullptr_ident);
    }

    cstmt_block_add(block, cstmt_expr(call_handler));
}

CStmt idl_cgen_dispatch_body(IdlIface const iface, Alloc *alloc)
{
    CStmt block = cstmt_block(alloc);

    CStmt dispatch_body = cstmt_block(alloc);

    vec_foreach_v(method, &iface.methods)
    {
        CStmt case_body = cstmt_block(alloc);

        cstmt_block_add(&dispatch_body, cstmt_case(cexpr_ident(fmt_str$(alloc, "MSG_{case-upper}_REQ", method.name))));
        idl_cgen_dispatch_case(&case_body, method, iface, alloc);
        cstmt_block_add(&case_body, cstmt_break());
        cstmt_block_add(&dispatch_body, case_body);
    }

    CStmt dispatch_switch = cstmt_switch(cexpr_ptr_access(cexpr_ident(str$("req")), cexpr_ident(str$("type")), alloc), dispatch_body, alloc);
    cstmt_block_add(&block, dispatch_switch);

    return block;
}

CDecl idl_cgen_dispatch_func(IdlIface const iface, Alloc *alloc)
{
    Str name = fmt_str$(alloc, "__IDL_PRIVATE__{case-snake}_dispatch_rpc", iface.name);
    CType type = idl_cgen_dispatch_type(alloc);
    CStmt body = idl_cgen_dispatch_body(iface, alloc);

    return cdecl_attrib(cdecl_func(name, type, body), CDECL_STATIC);
}
