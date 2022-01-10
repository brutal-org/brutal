#include <brutal/io.h>
#include <cc/builder.h>
#include <idl/cgen.h>

CType idl_cgen_dispatch_type(Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);

    ctype_member(&ctype, str$("ev"), ctype_ident_ptr(str$("IpcComponent"), alloc));
    ctype_member(&ctype, str$("req"), ctype_ident_ptr(str$("BrMsg"), alloc));
    ctype_member(&ctype, str$("ctx"), ctype_ptr(ctype_void(), alloc));

    return ctype;
}

CExpr idl_cgen_dispatch_handler(IdlMethod method, Alloc *alloc)
{
    CExpr expr = cexpr_initializer(alloc);

    Str handlerType;

    if (method.request.type == IDL_TYPE_NIL && method.response.type == IDL_TYPE_NIL)
    {
        handlerType = str$("IDL_HANDLER_NIL_NIL");
    }
    else if (method.request.type != IDL_TYPE_NIL && method.response.type == IDL_TYPE_NIL)
    {
        handlerType = str$("IDL_HANDLER_REQ_NIL");
    }
    else if (method.request.type == IDL_TYPE_NIL && method.response.type != IDL_TYPE_NIL)
    {
        handlerType = str$("IDL_HANDLER_NIL_RESP");
    }
    else
    {
        handlerType = str$("IDL_HANDLER_REQ_RESP");
    }

    cexpr_member(&expr, cexpr_ident(handlerType));
    CExpr vtable_field = cexpr_ptr_access(cexpr_ident(str$("vtable")), cexpr_ident(method.name), alloc);

    cexpr_member(&expr, cexpr_cast(vtable_field, ctype_ident_ptr(str$("IdlHandlerFn"), alloc), alloc));

    return cexpr_cast(expr, ctype_ident(str$("IdlHandler")), alloc);
}

void idl_cgen_dispatch_case(CStmt *block, IdlMethod method, IdlIface const iface, Alloc *alloc)
{
    CExpr call_handler = cexpr_call(alloc, cexpr_ident(str$("ipc_hook_handle")));

    cexpr_member(&call_handler, idl_cgen_dispatch_handler(method, alloc));
    cexpr_member(&call_handler, cexpr_ident(str$("ev")));
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
    CType vtable_type = ctype_ident_ptr(str_fmt(alloc, "{}VTable", iface.name), alloc);

    cstmt_block_add(&block, cstmt_decl(cdecl_var(str$("vtable"), vtable_type, cexpr_cast(cexpr_ident(str$("ctx")), vtable_type, alloc)), alloc));

    CStmt dispatch_body = cstmt_block(alloc);

    vec_foreach_v(method, &iface.methods)
    {
        CStmt case_body = cstmt_block(alloc);

        cstmt_block_add(&dispatch_body, cstmt_case(cexpr_ident(str_fmt(alloc, "MSG_{case:upper}_REQ", method.mangled))));
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
    Str name = str_fmt(alloc, "{case:snake}_dispatch", iface.name);
    CType type = idl_cgen_dispatch_type(alloc);
    CStmt body = idl_cgen_dispatch_body(iface, alloc);

    return cdecl_func(name, type, body);
}
