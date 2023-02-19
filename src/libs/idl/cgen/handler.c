#include <cc/builder.h>
#include <idl/cgen.h>
#include <brutal-io>

CType idl_cgen_handler_type(IdlMethod method, IdlModule const module, Alloc *alloc)
{
    CType ctype = ctype_func(idl_cgen_decl_primitive(module.errors), alloc);

    ctype_member(&ctype, str$("self"), ctype_ptr(ctype_void(), alloc));

    CType req_type = ctype_ptr(ctype_void(), alloc);
    if (method.request.type != IDL_TYPE_NIL)
    {
        req_type = ctype_ptr(ctype_attr(idl_cgen_decl_primitive(method.request), CTYPE_CONST), alloc);
    }
    ctype_member(&ctype, str$("req"), req_type);

    CType res_type = ctype_ptr(ctype_void(), alloc);
    if (method.response.type != IDL_TYPE_NIL)
    {
        res_type = ctype_ptr(idl_cgen_decl_primitive(method.response), alloc);
    }
    ctype_member(&ctype, str$("resp"), res_type);

    ctype_member(&ctype, str$("alloc"), ctype_ident_ptr(str$("Alloc"), alloc));

    return ctype;
}
