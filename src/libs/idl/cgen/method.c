#include <cc/builder.h>
#include <idl/cgen.h>
#include <brutal-io>

CType idl_cgen_method_type(IdlMethod method, IdlModule const module, Alloc *alloc)
{
    CType ctype = ctype_func(idl_cgen_decl_primitive(module.errors), alloc);

    ctype_member(&ctype, str$("ipc"), ctype_ident_ptr(str$("IpcComponent"), alloc));

    ctype_member(&ctype, str$("cap"), ctype_ident(str$("IpcCap")));

    if (method.request.type != IDL_TYPE_NIL)
    {
        CType req_type = ctype_ptr(ctype_attr(idl_cgen_decl_primitive(method.request), CTYPE_CONST), alloc);
        ctype_member(&ctype, str$("req"), req_type);
    }

    if (method.response.type != IDL_TYPE_NIL)
    {
        CType res_type = ctype_ptr(idl_cgen_decl_primitive(method.response), alloc);
        ctype_member(&ctype, str$("resp"), res_type);
    }

    if (method.response.type != IDL_TYPE_NIL || method.request.type != IDL_TYPE_NIL)
    {
        ctype_member(&ctype, str$("alloc"), ctype_ident_ptr(str$("Alloc"), alloc));
    }

    return ctype;
}

CStmt idl_cgen_method_body(IdlMethod method, IdlIface const iface, Alloc *alloc)
{
    CStmt block = cstmt_block(alloc);

    CExpr pack_request = cexpr_call(alloc, cexpr_ident(str$("ipc_hook_call")));
    cexpr_member(&pack_request, cexpr_ident(str$("ipc")));
    cexpr_member(&pack_request, cexpr_ident(str$("cap")));
    cexpr_member(&pack_request, idl_cgen_binding(method, iface, alloc));

    CExpr nullptr_ident = cexpr_ident(str$("nullptr"));

    if (method.request.type != IDL_TYPE_NIL)
    {
        cexpr_member(&pack_request, cexpr_ident(str$("req")));
    }
    else
    {
        cexpr_member(&pack_request, nullptr_ident);
    }

    if (method.response.type != IDL_TYPE_NIL)
    {
        cexpr_member(&pack_request, cexpr_ident(str$("resp")));
    }
    else
    {
        cexpr_member(&pack_request, nullptr_ident);
    }

    if (method.request.type != IDL_TYPE_NIL ||
        method.response.type != IDL_TYPE_NIL)
    {
        cexpr_member(&pack_request, cexpr_ident(str$("alloc")));
    }
    else
    {
        cexpr_member(&pack_request, nullptr_ident);
    }

    cstmt_block_add(&block, cstmt_return(pack_request));

    return block;
}
