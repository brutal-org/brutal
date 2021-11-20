#include <bid/builtin.h>
#include <bid/gen/c.h>
#include <brutal/debug.h>
#include <cc/builder.h>

static CType gen_decl_type(BidType type, Alloc *alloc);

/* --- C Header ----------------------------------------- */

static CType gen_decl_primitive(BidType type)
{
    BidBuiltinType *builtin = bid_lookup_builtin(type.primitive_.name);

    if (builtin != nullptr)
    {
        return ctype_ident(builtin->cname);
    }

    return ctype_ident(type.primitive_.mangled);
}

static CType gen_decl_enum(BidType type, Alloc *alloc)
{
    CType ctype = ctype_enum(alloc);

    int i = 0;
    vec_foreach(member, &type.enum_.members)
    {
        ctype_constant(&ctype, member.mangled, cval_signed(i++));
    }

    return ctype;
}

static CType gen_decl_struct(BidType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);

    vec_foreach(member, &type.struct_.members)
    {
        ctype_member(&ctype, member.name, gen_decl_type(member.type, alloc));
    }

    return ctype;
}

static CType gen_decl_vec(BidType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);
    CType buf_type = ctype_ptr(gen_decl_type(*type.vec_.subtype, alloc), alloc);
    CType size_type = ctype_ident(str$("size_t"));

    ctype_member(&ctype, str$("buf"), buf_type);
    ctype_member(&ctype, str$("len"), size_type);

    return ctype;
}

static CType gen_decl_type(BidType type, Alloc *alloc)
{
    switch (type.type)
    {
    case BID_TYPE_NIL:
        return ctype_void();

    case BID_TYPE_PRIMITIVE:
        return gen_decl_primitive(type);

    case BID_TYPE_ENUM:
        return gen_decl_enum(type, alloc);

    case BID_TYPE_STRUCT:
        return gen_decl_struct(type, alloc);

    case BID_TYPE_VEC:
        return gen_decl_vec(type, alloc);

    default:
        panic$("Unknow type type {}", type.type);
    }
}

static CType gen_type(BidType type)
{
    assert_truth(type.type == BID_TYPE_PRIMITIVE);
    return ctype_ident(type.primitive_.mangled);
}

static CType gen_func_method(BidMethod method, BidIface const iface, Alloc *alloc)
{
    CType ctype = ctype_func(gen_type(iface.errors), alloc);

    ctype_member(&ctype, str$("ev"), ctype_ident_ptr(str$("IpcEv"), alloc));

    ctype_member(&ctype, str$("task"), ctype_ident(str$("BrTask")));

    if (method.request.type != BID_TYPE_NIL)
    {
        CType req_type = ctype_ptr(ctype_attr(gen_type(method.request), CTYPE_CONST), alloc);
        ctype_member(&ctype, str$("req"), req_type);
    }

    if (method.response.type != BID_TYPE_NIL)
    {
        CType res_type = ctype_ptr(gen_type(method.response), alloc);
        ctype_member(&ctype, str$("resp"), res_type);
    }

    if (method.response.type != BID_TYPE_NIL || method.request.type != BID_TYPE_NIL)
    {
        ctype_member(&ctype, str$("alloc"), ctype_ident_ptr(str$("Alloc"), alloc));
    }

    return ctype;
}

static CType gen_impl_type(BidIface const iface, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);
    CType vtable_type = ctype_ident(str_fmt(alloc, "{}VTable", iface.name));
    CType ipc_type = ctype_ident(str$("IpcEv"));

    ctype_member(&ctype, str$("ev"), ctype_ptr(ipc_type, alloc));
    ctype_member(&ctype, str$("vtable"), ctype_ptr(vtable_type, alloc));
    return ctype;
}

CUnit bidgen_c_header(MAYBE_UNUSED BidIface const iface, Alloc *alloc)
{
    CUnit unit = cunit(alloc);
    cunit_pragma_once(&unit);

    cunit_include(&unit, true, str$("bal/ipc.h"));

    vec_foreach(alias, &iface.aliases)
    {
        cunit_decl(&unit, cdecl_type(alias.mangled, gen_decl_type(alias.type, alloc)));
    }

    CType vtable = ctype_struct(alloc);
    CType msgtype = ctype_enum(alloc);

    int i = 0;
    vec_foreach(method, &iface.methods)
    {
        Str name = case_change_str(CASE_PASCAL, method.mangled, alloc);
        CType type = gen_func_method(method, iface, alloc);

        ctype_member(&vtable, method.name, ctype_ident_ptr(name, alloc));
        cunit_decl(&unit, cdecl_type(name, type));
        cunit_decl(&unit, cdecl_func(method.mangled, type, cstmt_empty()));

        ctype_constant(&msgtype, str_fmt(alloc, "MSG_{case:constant}_REQ", method.mangled), cval_unsigned(i++));
        ctype_constant(&msgtype, str_fmt(alloc, "MSG_{case:constant}_RESP", method.mangled), cval_unsigned(i++));
    }

    cunit_decl(&unit, cdecl_type(str_fmt(alloc, "{}Msgs", iface.name), msgtype));
    cunit_decl(&unit, cdecl_type(str_fmt(alloc, "{}VTable", iface.name), vtable));
    cunit_decl(&unit, cdecl_func(str_fmt(alloc, "{case:lower}_impl", iface.name), gen_impl_type(iface, alloc), cstmt_empty()));

    return unit;
}

/* --- C Source ----------------------------------------- */

Str gen_pack_name(Str name, Alloc *alloc)
{
    BidBuiltinType *builtin = bid_lookup_builtin(name);

    if (builtin)
    {
        return builtin->pack_fn;
    }

    return str_fmt(alloc, "{case:snake}_pack", name);
}

CExpr gen_pack_ref(Str name, Alloc *alloc)
{
    return cexpr_cast(cexpr_ident(gen_pack_name(name, alloc)), ctype_ident_ptr(str$("BalPackFn"), alloc), alloc);
}

Str gen_unpack_name(Str name, Alloc *alloc)
{
    BidBuiltinType *builtin = bid_lookup_builtin(name);

    if (builtin)
    {
        return builtin->unpack_fn;
    }

    return str_fmt(alloc, "{case:snake}_unpack", name);
}

CExpr gen_unpack_ref(Str name, Alloc *alloc)
{
    return cexpr_cast(cexpr_ident(gen_unpack_name(name, alloc)), ctype_ident_ptr(str$("BalUnpackFn"), alloc), alloc);
}

CType gen_pack_type(BidAlias alias, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);
    ctype_member(&ctype, str$("self"), ctype_ident_ptr(str$("BalPack"), alloc));
    ctype_member(&ctype, str$("data"), ctype_ident_ptr(alias.mangled, alloc));
    return ctype;
}

CType gen_unpack_type(BidAlias alias, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);
    ctype_member(&ctype, str$("self"), ctype_ident_ptr(str$("BalUnpack"), alloc));
    ctype_member(&ctype, str$("data"), ctype_ident_ptr(alias.mangled, alloc));
    return ctype;
}

void gen_pack_body(CStmt *block, BidType type, CExpr path, Alloc *alloc)
{
    switch (type.type)
    {
    case BID_TYPE_NIL:
        break;

    case BID_TYPE_PRIMITIVE:
    {
        CExpr expr = cexpr_call(alloc, cexpr_ident(gen_pack_name(type.primitive_.mangled, alloc)));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);

        cstmt_block_add(block, cstmt_expr(expr));
    }
    break;

    case BID_TYPE_ENUM:
    {
        CExpr expr = cexpr_call(alloc, cexpr_ident(gen_pack_name(str$("Enum"), alloc)));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, cexpr_cast(path, ctype_ident_ptr(str$("int"), alloc), alloc));

        cstmt_block_add(block, cstmt_expr(expr));
    }
    break;

    case BID_TYPE_STRUCT:
    {
        vec_foreach(member, &type.struct_.members)
        {
            gen_pack_body(block, member.type, cexpr_ref(cexpr_ptr_access(path, cexpr_ident(member.name), alloc), alloc), alloc);
        }
    }
    break;

    case BID_TYPE_VEC:
    {
        BidType subtype = *type.vec_.subtype;
        assert_truth(subtype.type == BID_TYPE_PRIMITIVE);
        CExpr expr = cexpr_call(alloc, cexpr_ident(str$("bal_pack_slice")));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);
        cexpr_member(&expr, cexpr_ident(gen_pack_name(subtype.primitive_.mangled, alloc)));
        cstmt_block_add(block, cstmt_expr(expr));
    }

    break;
    default:
        panic$("Unknow type type {}", type.type);
    }
}

void gen_unpack_body(CStmt *block, BidType type, CExpr path, Alloc *alloc)
{
    switch (type.type)
    {
    case BID_TYPE_NIL:
        break;

    case BID_TYPE_PRIMITIVE:
    {
        CExpr expr = cexpr_call(alloc, cexpr_ident(gen_unpack_name(type.primitive_.mangled, alloc)));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);

        cstmt_block_add(block, cstmt_expr(expr));
    }
    break;

    case BID_TYPE_ENUM:
    {
        CExpr expr = cexpr_call(alloc, cexpr_ident(gen_unpack_name(str$("Enum"), alloc)));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, cexpr_cast(path, ctype_ident_ptr(str$("int"), alloc), alloc));

        cstmt_block_add(block, cstmt_expr(expr));
    }
    break;

    case BID_TYPE_STRUCT:
    {
        vec_foreach(member, &type.struct_.members)
        {
            gen_unpack_body(block, member.type, cexpr_ref(cexpr_ptr_access(path, cexpr_ident(member.name), alloc), alloc), alloc);
        }
    }
    break;

    case BID_TYPE_VEC:
    {
        BidType subtype = *type.vec_.subtype;
        assert_truth(subtype.type == BID_TYPE_PRIMITIVE);

        CExpr expr = cexpr_call(alloc, cexpr_ident(str$("bal_unpack_slice")));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);
        cexpr_member(&expr, cexpr_ident(gen_unpack_name(subtype.primitive_.mangled, alloc)));
        cstmt_block_add(block, cstmt_expr(expr));
    }
    break;

    default:
        panic$("Unknow type type {}", type.type);
    }
}

CDecl gen_pack_func(BidAlias alias, Alloc *alloc)
{
    Str name = gen_pack_name(alias.mangled, alloc);
    CType type = gen_pack_type(alias, alloc);
    CStmt body = cstmt_block(alloc);
    gen_pack_body(&body, alias.type, cexpr_ident(str$("data")), alloc);

    return cdecl_func(name, type, body);
}

CDecl gen_unpack_func(BidAlias alias, Alloc *alloc)
{
    Str name = gen_unpack_name(alias.mangled, alloc);
    CType type = gen_unpack_type(alias, alloc);
    CStmt body = cstmt_block(alloc);
    gen_unpack_body(&body, alias.type, cexpr_ident(str$("data")), alloc);

    return cdecl_func(name, type, body);
}

CStmt gen_method_body(BidMethod method, BidIface const iface, Alloc *alloc)
{
    CStmt block = cstmt_block(alloc);

    CExpr pack_request = cexpr_call(alloc, cexpr_ident(str$("bid_hook_call")));
    cexpr_member(&pack_request, cexpr_ident(str$("ev")));
    cexpr_member(&pack_request, cexpr_ident(str$("task")));
    cexpr_member(&pack_request, cexpr_constant(cval_signed(iface.id)));

    cexpr_member(&pack_request, cexpr_ident(str_fmt(alloc, "MSG_{case:constant}_REQ", method.mangled)));

    CExpr nullptr_ident = cexpr_ident(str$("nullptr"));

    if (method.request.type != BID_TYPE_NIL)
    {
        cexpr_member(&pack_request, cexpr_ident(str$("req")));
        cexpr_member(&pack_request, gen_pack_ref(method.request.primitive_.mangled, alloc));
    }
    else
    {
        cexpr_member(&pack_request, nullptr_ident);
        cexpr_member(&pack_request, nullptr_ident);
    }

    cexpr_member(&pack_request, cexpr_ident(str_fmt(alloc, "MSG_{case:constant}_RESP", method.mangled)));

    if (method.response.type != BID_TYPE_NIL)
    {
        cexpr_member(&pack_request, cexpr_ident(str$("resp")));
        cexpr_member(&pack_request, gen_unpack_ref(method.response.primitive_.mangled, alloc));
    }
    else
    {
        cexpr_member(&pack_request, nullptr_ident);
        cexpr_member(&pack_request, nullptr_ident);
    }

    if (method.request.type != BID_TYPE_NIL || method.response.type != BID_TYPE_NIL)
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

CType gen_dispatch_type(Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);

    ctype_member(&ctype, str$("ev"), ctype_ident_ptr(str$("IpcEv"), alloc));
    ctype_member(&ctype, str$("req"), ctype_ident_ptr(str$("BrMsg"), alloc));
    ctype_member(&ctype, str$("ctx"), ctype_ptr(ctype_void(), alloc));

    return ctype;
}

CExpr gen_dispatch_handler(BidMethod method, Alloc *alloc)
{
    CExpr expr = cexpr_initializer(alloc);

    Str handlerType;

    if (method.request.type == BID_TYPE_NIL && method.response.type == BID_TYPE_NIL)
    {
        handlerType = str$("BID_HANDLER_NIL_NIL");
    }
    else if (method.request.type != BID_TYPE_NIL && method.response.type == BID_TYPE_NIL)
    {
        handlerType = str$("BID_HANDLER_REQ_NIL");
    }
    else if (method.request.type == BID_TYPE_NIL && method.response.type != BID_TYPE_NIL)
    {
        handlerType = str$("BID_HANDLER_NIL_RESP");
    }
    else
    {
        handlerType = str$("BID_HANDLER_REQ_RESP");
    }

    cexpr_member(&expr, cexpr_ident(handlerType));
    CExpr vtable_field = cexpr_ptr_access(cexpr_ident(str$("vtable")), cexpr_ident(method.name), alloc);

    cexpr_member(&expr, cexpr_cast(vtable_field, ctype_ident_ptr(str$("BidHandlerFn"), alloc), alloc));

    return cexpr_cast(expr, ctype_ident(str$("BidHandler")), alloc);
}

void gen_dispatch_case(CStmt *block, BidMethod method, Alloc *alloc)
{
    CExpr call_handler = cexpr_call(alloc, cexpr_ident(str$("bid_hook_handle")));

    cexpr_member(&call_handler, gen_dispatch_handler(method, alloc));
    cexpr_member(&call_handler, cexpr_ident(str$("ev")));
    cexpr_member(&call_handler, cexpr_ident(str$("req")));

    CExpr nullptr_ident = cexpr_ident(str$("nullptr"));

    if (method.request.type != BID_TYPE_NIL)
    {
        cstmt_block_add(block, cstmt_decl(cdecl_var(str$("req_buf"), ctype_ident(method.request.primitive_.mangled), cexpr_empty()), alloc));

        cexpr_member(&call_handler, cexpr_ref(cexpr_ident(str$("req_buf")), alloc));
        cexpr_member(&call_handler, gen_unpack_ref(method.request.primitive_.mangled, alloc));
    }
    else
    {
        cexpr_member(&call_handler, nullptr_ident);
        cexpr_member(&call_handler, nullptr_ident);
    }

    if (method.response.type != BID_TYPE_NIL)
    {
        cstmt_block_add(block, cstmt_decl(cdecl_var(str$("resp_buf"), ctype_ident(method.response.primitive_.mangled), cexpr_empty()), alloc));

        cexpr_member(&call_handler, cexpr_ref(cexpr_ident(str$("resp_buf")), alloc));
        cexpr_member(&call_handler, cexpr_ident(str_fmt(alloc, "MSG_{case:constant}_RESP", method.mangled)));
        cexpr_member(&call_handler, gen_pack_ref(method.response.primitive_.mangled, alloc));
    }
    else
    {
        cexpr_member(&call_handler, nullptr_ident);
        cexpr_member(&call_handler, cexpr_constant(cval$(-1)));
        cexpr_member(&call_handler, nullptr_ident);
    }

    cstmt_block_add(block, cstmt_expr(call_handler));
}

CStmt gen_dispatch_body(BidIface const iface, Alloc *alloc)
{
    CStmt block = cstmt_block(alloc);
    CType vtable_type = ctype_ident_ptr(str_fmt(alloc, "{}VTable", iface.name), alloc);

    cstmt_block_add(&block, cstmt_decl(cdecl_var(str$("vtable"), vtable_type, cexpr_cast(cexpr_ident(str$("ctx")), vtable_type, alloc)), alloc));

    CStmt dispatch_body = cstmt_block(alloc);

    vec_foreach(method, &iface.methods)
    {
        CStmt case_body = cstmt_block(alloc);

        cstmt_block_add(&dispatch_body, cstmt_case(cexpr_ident(str_fmt(alloc, "MSG_{case:upper}_REQ", method.mangled))));
        gen_dispatch_case(&case_body, method, alloc);
        cstmt_block_add(&dispatch_body, case_body);
        cstmt_block_add(&dispatch_body, cstmt_break());
    }

    CStmt dispatch_switch = cstmt_switch(cexpr_ptr_access(cexpr_ident(str$("req")), cexpr_ident(str$("type")), alloc), dispatch_body, alloc);
    cstmt_block_add(&block, dispatch_switch);

    return block;
}

CDecl gen_dispatch_func(BidIface const iface, Alloc *alloc)
{
    Str name = str_fmt(alloc, "{case:snake}_dispatch", iface.name);
    CType type = gen_dispatch_type(alloc);
    CStmt body = gen_dispatch_body(iface, alloc);

    return cdecl_func(name, type, body);
}

CDecl gen_impl_func(BidIface const iface, Alloc *alloc)
{
    Str name = str_fmt(alloc, "{case:lower}_impl", iface.name);
    CType type = gen_impl_type(iface, alloc);

    CStmt body = cstmt_block(alloc);
    CExpr expr = cexpr_call(alloc, cexpr_ident(str$("br_ev_impl")));

    // void br_ev_impl(IpcEv *self, uint32_t id, IpcFn *fn, void *ctx);

    cexpr_member(&expr, cexpr_ident(str$("ev")));
    cexpr_member(&expr, cexpr_constant(cval_unsigned(iface.id)));
    cexpr_member(&expr, cexpr_ident(str_fmt(alloc, "{case:snake}_dispatch", iface.name)));
    cexpr_member(&expr, cexpr_ident(str$("vtable")));

    cstmt_block_add(&body, cstmt_expr(expr));

    return cdecl_func(name, type, body);
}

CUnit bidgen_c_source(MAYBE_UNUSED BidIface const iface, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    cunit_include(&unit, false, str_fmt(alloc, "{case:snake}.h", iface.name));

    vec_foreach(alias, &iface.aliases)
    {
        cunit_decl(&unit, gen_pack_func(alias, alloc));
        cunit_decl(&unit, gen_unpack_func(alias, alloc));
    }

    vec_foreach(method, &iface.methods)
    {
        CType type = gen_func_method(method, iface, alloc);
        CStmt body = gen_method_body(method, iface, alloc);

        cunit_decl(&unit, cdecl_func(method.mangled, type, body));
    }

    cunit_decl(&unit, gen_dispatch_func(iface, alloc));
    cunit_decl(&unit, gen_impl_func(iface, alloc));

    return unit;
}
