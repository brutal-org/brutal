#include <bid/bid.h>
#include <cc/builder.h>
#include <cc/builder/helper.h>
static CType emit_type(BidInterface const *interface, BidType const *type, Alloc *alloc);

void bid_unit_add_typedef(CUnit *unit, BidInterface const *interface, Str s, BidType type, Alloc *alloc)
{
    CType t = emit_type(interface, &type, alloc);
    CDecl type_def = cdecl_type(s, t, alloc);
    cunit_member(unit, cunit_decl(type_def));
}

static bool type_in_interface(BidInterface const *interface, Str name)
{
    vec_foreach(type, &interface->aliases)
    {
        if (str_eq(type.name, name))
        {
            return true;
        }
    }

    return false;
}

static CType emit_primitive(BidInterface const *interface, BidPrimitive const *type, Alloc *alloc)
{

    Str res;
    if (type_in_interface(interface, type->name))
    {
        res = str_fmt(alloc, "{case:pascal}{}", interface->name, type->name);
    }
    else
    {
        res = str_fmt(alloc, "{}", type->name);
    }

    return ctype_name(res, alloc);
}

static CType emit_enum(BidInterface const *interface, BidEnum const *type, Alloc *alloc)
{
    CType enum_type = ctype_enum(str$(""), alloc);

    vec_foreach(member, &type->members)
    {

        Str s = str_fmt(alloc, "{case:upper}_{case:upper}", interface->name, member);
        ctype_constant_no_value(&enum_type, s, alloc);

        alloc_free(alloc, s.buffer);
    }
    return enum_type;
}

static CType emit_struct(BidInterface const *interface, BidStruct const *type, Alloc *alloc)
{
    CType cstruct = ctype_struct(str$(""), alloc);
    vec_foreach(member, &type->members)
    {
        ctype_member(&cstruct,
                     member.name,
                     emit_type(interface, &member.type, alloc),
                     alloc);
    }

    return cstruct;
}

static CType emit_type(BidInterface const *interface, BidType const *type, Alloc *alloc)
{
    switch (type->type)
    {
    case BID_TYPE_PRIMITIVE:
        return emit_primitive(interface, &type->primitive_, alloc);

    case BID_TYPE_ENUM:
        return emit_enum(interface, &type->enum_, alloc);

    case BID_TYPE_STRUCT:
        return emit_struct(interface, &type->struct_, alloc);

    default:
        panic$("invalid type");
    }
}

static bool is_handle(BidType *type)
{
    if (type->type != BID_TYPE_PRIMITIVE)
    {
        return false;
    }

    return str_eq(type->primitive_.name, str$("BrHandle")) ||
           str_eq(type->primitive_.name, str$("BrSpace")) ||
           str_eq(type->primitive_.name, str$("BrMemObj")) ||
           str_eq(type->primitive_.name, str$("BrTask")) ||
           str_eq(type->primitive_.name, str$("BrIrq"));
}

static CExpr bid2c_method_flags(BidType request_type, Alloc *alloc)
{
    switch (request_type.type)
    {
    case BID_TYPE_PRIMITIVE:
        if (is_handle(&request_type))
        {
            CExpr call = cexpr_call(alloc, cexpr_identifier(str$("BR_MSG_HND"), alloc));
            cexpr_add_arg(&call, cexpr_constant(cval_signed(0)));
            return (call);
        }
        else
        {
            return cexpr_constant(cval_signed(0));
        }

    case BID_TYPE_ENUM:
        return cexpr_constant(cval_signed(0));

    case BID_TYPE_STRUCT:
    {
        CExpr v = cexpr_constant(cval_signed(0));

        int index = 0;
        vec_foreach(member, &request_type.struct_.members)
        {
            if (is_handle(&member.type) | member.kernel_handle)
            {
                // | BR_MSG_HND(index)
                CExpr call = cexpr_call(alloc, cexpr_identifier(str$("BR_MSG_HND"), alloc));
                cexpr_add_arg(&call, cexpr_constant(cval_signed(index)));
                v = cexpr_bit_or(v, call, alloc);
            }
            index++;
        }
    }

    default:
        return cexpr_constant(cval_signed(0));
    }
}
static CExpr bid2c_msg_args(BidType request_type, Alloc *alloc)
{
    CExpr arr_init = cexpr_struct_initializer(alloc);
    switch (request_type.type)
    {
    case BID_TYPE_PRIMITIVE:
    case BID_TYPE_ENUM:
    {
        // (BrArg)req
        CExpr targ = cexpr_cast(
            cexpr_identifier(str$("req"), alloc),
            ctype_name(str$("BrArg"), alloc),
            alloc);

        cexpr_initializer_push_element(&arr_init, cexpr_empty(), targ, alloc);
    }

    case BID_TYPE_STRUCT:
        vec_foreach(member, &request_type.struct_.members)
        {
            // (BrArg)req->{member.name}
            CExpr targ = cexpr_cast(
                cexpr_ptr_access(
                    cexpr_identifier(str$("req"), alloc),
                    cexpr_identifier(member.name, alloc),
                    alloc),
                ctype_name(str$("BrArg"), alloc),
                alloc);

            cexpr_initializer_push_element(&arr_init, cexpr_empty(), targ, alloc);
        }
        break;

    default:
        return cexpr_empty();
        break;
    }

    return arr_init;
}

static CStmt bid2c_msg_create(BidInterface const *interface, BidMethod const *method, Alloc *alloc)
{

    CExpr msg_init = cexpr_struct_initializer(alloc);

    Str protocol_id = str_fmt(alloc, "{case:upper}_PROTOCOL_ID", interface->name);
    cc_push_initializer_member(&msg_init, str$("prot"), protocol_id, alloc);

    Str request = str_fmt(alloc, "{case:upper}_{case:upper}_REQUEST", interface->name, method->name);
    cc_push_initializer_member(&msg_init, str$("type"), request, alloc);

    if (method->request.type != BID_TYPE_NONE)
    {
        // .arg = {}
        CExpr get = cexpr_access(cexpr_empty(), cexpr_identifier(str$("arg"), alloc), alloc);
        CExpr args = bid2c_msg_args(method->request, alloc);
        cexpr_initializer_push_element(&msg_init, get, args, alloc);

        // .flags = {}
        get = cexpr_access(cexpr_empty(), cexpr_identifier(str$("flags"), alloc), alloc);
        CExpr flags = bid2c_method_flags(method->request, alloc);
        cexpr_initializer_push_element(&msg_init, get, flags, alloc);
    }
    CStmt res = cc_var_decl_str(str$("BrMsg"), str$("req_msg"), msg_init, alloc);
    return res;
}

static CDecl bid2c_method_decl(BidInterface const *interface, BidMethod const *method, CStmt func_stmt, Alloc *alloc)
{

    Str err_type = str_fmt(alloc, "{case:pascal}Error", interface->name);
    Str func_name = str_fmt(alloc, "{case:lower}_{case:lower}", interface->name, method->name);

    CType ret_type = ctype_name(err_type, alloc);
    CType func_type = ctype_func(ret_type, func_name, alloc);

    ctype_member(&func_type, str$("task"), ctype_name(str$("BrId"), alloc), alloc);
    if (method->request.type != BID_TYPE_NONE)
    {
        Str request_type = str_fmt(alloc, "{case:pascal}{case:pascal}Request", interface->name, method->name);
        CType req_type = ctype_ptr(ctype_attr(ctype_name(request_type, alloc), CTYPE_CONST), alloc);
        ctype_member(&func_type, str$("req"), req_type, alloc);
    }

    if (method->response.type != BID_TYPE_NONE)
    {
        Str response_type = str_fmt(alloc, "{case:pascal}{case:pascal}Response", interface->name, method->name);
        CType res_type = ctype_ptr(ctype_name(response_type, alloc), alloc);
        ctype_member(&func_type, str$("resp"), res_type, alloc);
    }

    CDecl func = cdecl_func(func_name, func_type, func_stmt, alloc);

    return cdecl_attrib(func, CDECL_INLINE | CDECL_STATIC);
}

static CExpr bid2c_br_ev_eq_call(Alloc *alloc)
{
    CExpr call = cc_func_call(str$("br_ev_req"), alloc);
    cexpr_add_arg(&call, cexpr_identifier(str$("task"), alloc));
    cexpr_add_arg(&call, cexpr_ref(cexpr_identifier(str$("req_msg"), alloc), alloc));
    cexpr_add_arg(&call, cexpr_ref(cexpr_identifier(str$("resp_msg"), alloc), alloc));
    return call;
}

static void bid2c_msg_handle(CStmt *targ, BidInterface const *interface, BidMethod const *method, Alloc *alloc)
{

    /*
    if (result != BR_SUCCESS)
    {
        return {}_BAD_COMMUNICATION
    }
    */

    Str bad_communication_str = str_fmt(alloc, "{case:upper}_BAD_COMMUNICATION", interface->name);

    CStmt stmt = cstmt_if(
        cexpr_str_op(not_eq, str$("result"), str$("BR_SUCCESS"), alloc),
        cstmt_return(cexpr_identifier(bad_communication_str, alloc)), // return bad communication str
        alloc);

    cstmt_block_add(targ, stmt);

    /*
    "if (resp_msg.prot != protocol_id || "
                   "(resp_msg.type != response_id && resp_msg.type != error_id))\n"
                   "{{\n"
                   "return INTERFACE_UNEXPECTED_MESSAGE;\n"
                   "}}\n",
    */

    Str protocol_id_str = str_fmt(alloc, "{case:upper}_PROTOCOL_ID", interface->name);
    Str response_id_str = str_fmt(alloc, "{case:upper}_WRITE_RESPONSE", interface->name);
    Str error_id_str = str_fmt(alloc, "{case:upper}_ERROR", interface->name);
    Str unexpected_str = str_fmt(alloc, "{case:upper}_UNEXPECTED_MESSAGE", interface->name);

    CExpr cexpr_resp_access_type = cc_access_str(str$("resp_msg"), str$("type"), alloc); // resp_msg.type

    CExpr resp_or_error_cond =
        cexpr_or( // ||
            cexpr_not_eq(
                cc_access_str(str$("resp_msg"), str$("prot"), alloc), // resp_msg.prot
                cexpr_identifier(protocol_id_str, alloc),
                alloc),
            cexpr_and( // &&
                // resp_msg.type != response_id
                cexpr_not_eq(
                    cexpr_resp_access_type,
                    cexpr_identifier(response_id_str, alloc), alloc),
                // resp_msg.type != error_id
                cexpr_not_eq(
                    cexpr_resp_access_type,
                    cexpr_identifier(error_id_str, alloc), alloc),
                alloc),
            alloc);

    stmt = cstmt_if(resp_or_error_cond,
                    cstmt_return(cexpr_identifier(unexpected_str, alloc)), // return bad communication str
                    alloc);

    /*
    if (resp_msg.type == error)
    {
        return  (error_type)resp_msg.arg[0];
    }
    */
    Str err_type = str_fmt(alloc, " {case:pascal}Error", interface->name);

    CExpr ret_res = cexpr_cast(
        cc_index_constant(cc_access_str(str$("resp_msg"), str$("arg"), alloc), 0, alloc), // resp_msg.arg[0]
        ctype_name(err_type, alloc),
        alloc);

    stmt = cstmt_if(cexpr_eq(
                        cc_access_str(str$("resp_msg"), str$("type"), alloc),
                        cexpr_identifier(error_id_str, alloc),
                        alloc),
                    cstmt_return(ret_res), // return bad communication str
                    alloc);

    cstmt_block_add(targ, stmt);
}

static void bid2c_msg_resp_init(CStmt *targ, BidInterface const *interface, BidMethod const *method, Alloc *alloc)
{

    BidType response = method->response;

    Str request_type = str_fmt(alloc, "{case:pascal}{case:pascal}Request", interface->name, method->name);
    switch (response.type)
    {
    case BID_TYPE_PRIMITIVE:
    case BID_TYPE_ENUM:
    {
        CExpr set_expr = cexpr_assign(
            cexpr_deref(cexpr_identifier(str$("resp"), alloc), alloc),
            cexpr_cast(cc_access_str(str$("resp_msg"), str$("arg"), alloc), ctype_name(request_type, alloc), alloc), alloc);

        cstmt_block_add(targ, cstmt_expr(set_expr));
        break;
    }
    case BID_TYPE_STRUCT:
    {
        int index = 0;
        vec_foreach(member, &response.struct_.members)
        {
            // resp->{} = (typeof(resp->{}))resp_msg.arg[{}]
            CExpr set_expr = cexpr_assign(
                cc_ptr_access_str(str$("resp"), member.name, alloc),
                cexpr_cast(cc_index_constant(
                               cc_access_str(str$("resp_msg"), str$("arg"), alloc),
                               0, alloc),
                           emit_type(interface, &member.type, alloc),
                           alloc),
                alloc);

            cstmt_block_add(targ, cstmt_expr(set_expr));
            index++;
        }
    }
    break;

    default:
        break;
    }
}

static CDecl emit_method(BidInterface const *interface, BidMethod const *method, Alloc *alloc)
{

    PrintTrans response_id = print_trans(" {case:upper}_{case:upper}_RESPONSE", interface->name, method->name);
    PrintTrans err_id = print_trans(" {case:upper}_ERROR", interface->name);

    CStmt statement = cstmt_block(alloc);

    cstmt_block_add(&statement, bid2c_msg_create(interface, method, alloc));
    // BrMsg resp_msg = {};
    cstmt_block_add(&statement, cc_var_decl_str(str$("BrMsg"), str$("resp_msg"), cexpr_empty(), alloc));

    // BrResult result = br_ev_req(task, &req_msg, &resp_msg);
    cstmt_block_add(&statement, cc_var_decl_str(str$("BrResult"), str$("result"), bid2c_br_ev_eq_call(alloc), alloc));

    bid2c_msg_handle(&statement, interface, method, alloc);

    bid2c_msg_resp_init(&statement, interface, method, alloc);

    Str success_str = str_fmt(alloc, "{case:upper}_SUCCESS",
                              interface->name);

    cstmt_block_add(&statement, cstmt_return(cexpr_identifier(success_str, alloc))); // return {case:upper}_SUCCESS;

    CDecl func_decl = bid2c_method_decl(interface, method, statement, alloc);
    return func_decl;
}

static CDecl emit_callback(BidInterface const *interface, BidMethod const *method, Alloc *alloc)
{
    Str err_type = str_fmt(alloc, "{case:pascal}Error", interface->name);
    Str request_type = str_fmt(alloc, "{case:pascal}{case:pascal}Request", interface->name, method->name);
    Str response_type = str_fmt(alloc, "{case:pascal}{case:pascal}Response", interface->name, method->name);
    Str func_name = str_fmt(alloc, "{case:pascal}{case:pascal}Fn", interface->name, method->name);

    CType ctype_fn = ctype_func(ctype_name(err_type, alloc), str$(""), alloc);

    ctype_member(&ctype_fn, str$("from"), ctype_name(str$("BrId"), alloc), alloc); // BrId from

    if (method->request.type != BID_TYPE_NONE)
    {
        ctype_member(&ctype_fn, str$("req"),
                     ctype_ptr(ctype_attr(ctype_name(request_type, alloc), CTYPE_CONST), alloc), // request_type const *
                     alloc);                                                                     // BrId from
    }

    if (method->request.type != BID_TYPE_NONE)
    {
        ctype_member(&ctype_fn, str$("res"),
                     ctype_ptr(ctype_name(response_type, alloc), alloc), // request_type const *
                     alloc);                                             // BrId from
    }

    ctype_member(&ctype_fn, str$("ctx"),
                 ctype_ptr(ctype_name(str$("void"), alloc), alloc), // request_type const *
                 alloc);

    CDecl decl = cdecl_type(str$(func_name), ctype_fn, alloc);
}

/* --- Types ---------------------------------------------------------------- */
void bid2c_type(CUnit *unit, const BidInterface *interface, Alloc *alloc)
{
    vec_foreach(alias, &interface->aliases)
    {
        Str s = str_fmt(alloc, "{case:pascal}{case:pascal}", interface->name, alias.name);
        CDecl type_def = cdecl_type(s, emit_type(interface, &alias.type, alloc), alloc);
        cunit_member(unit, cunit_decl(type_def));
    }
}
/* --- Messages ------------------------------------------------------------- */

CType bid2c_method_message_type(BidInterface const *interface, Alloc *alloc)
{

    CType message_type_decl = ctype_enum(str$(""), alloc);

#define bid2c_const_msg_type(S)                 \
    ctype_constant_no_value(&message_type_decl, \
                            S,                  \
                            alloc);

    bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_INVALID,\n", interface->name));
    bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_ERROR,\n", interface->name));

    vec_foreach(method, &interface->methods)
    {
        bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_{case:upper}_RESPONSE,\n", interface->name, method.name));
        bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_{case:upper}_REQUEST,\n", interface->name, method.name));
    }

    vec_foreach(event, &interface->events)
    {
        bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_{case:upper}_EVENT,\n", interface->name, event.name));
    }

    return message_type_decl;
}
void bid2c_methods(CUnit *unit, BidInterface const *interface, Alloc *alloc)
{

    Str m_type_str = str_fmt(alloc, "{case:pascal}MessageType", interface->name);

    CDecl m_type_def = cdecl_type(m_type_str, bid2c_method_message_type(interface, alloc), alloc);
    cunit_member(unit, cunit_decl(m_type_def));

    vec_foreach(method, &interface->methods)
    {
        if (method.request.type != BID_TYPE_NONE)
        {
            Str s = str_fmt(alloc, "{case:pascal}{case:pascal}Request;", interface->name, method.name);
            bid_unit_add_typedef(unit, interface, s, method.request, alloc);
        }

        if (method.response.type != BID_TYPE_NONE)
        {
            Str s = str_fmt(alloc, "{case:pascal}{case:pascal}Response;", interface->name, method.name);
            bid_unit_add_typedef(unit, interface, s, method.response, alloc);
        }
    }

    vec_foreach(event, &interface->events)
    {
        Str s = str_fmt(alloc, "{case:pascal}{case:pascal}Event;", interface->name, event.name);
        bid_unit_add_typedef(unit, interface, s, event.data, alloc);
    }

    vec_foreach(method, &interface->methods)
    {
        cunit_member(unit, cunit_decl(emit_method(interface, &method, alloc)));
    }

    vec_foreach(method, &interface->methods)
    {
        cunit_member(unit, cunit_decl(emit_callback(interface, &method, alloc)));
    }

    emit_fmt(emit, "typedef void {case:pascal}ErrorFn(BrId from, BrResult error, void* ctx);\n", interface->name);

    emit_fmt(emit, "\n");

    emit_fmt(emit, "typedef struct {{\n");
    emit_ident(emit);

    emit_fmt(emit, "void* ctx;\n");

    vec_foreach(method, &interface->methods)
    {
        PrintTrans func_type = print_trans("{case:pascal}{case:pascal}Fn", interface->name, method.name);
        emit_fmt(emit, "{}* handle_{};\n", func_type, method.name);
    }

    emit_fmt(emit, "{case:pascal}ErrorFn* handle_error;\n", interface->name);

    emit_deident(emit);
    emit_fmt(emit, "}} {case:pascal}Server;\n", interface->name);

    emit_fmt(emit, "\n");

    emit_fmt(emit, "static inline void {case:lower}_server_dispatch({case:pascal}Server* server, BrMsg const* req_msg)\n", interface->name, interface->name);
    emit_fmt(emit, "{{\n");
    emit_ident(emit);

    emit_fmt(emit, "BrMsg resp_msg = {{}};\n\n");

    emit_fmt(emit, "switch (req_msg->type) {{\n");
    emit_ident(emit);

    vec_foreach(method, &interface->methods)
    {
        emit_fmt(emit, "case {case:upper}_{case:upper}_REQUEST:\n", interface->name, method.name);
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        PrintTrans request_type = print_trans("{case:pascal}{case:pascal}Request", interface->name, method.name);
        PrintTrans response_type = print_trans("{case:pascal}{case:pascal}Response", interface->name, method.name);

        emit_fmt(emit, "{} req = {{}};\n", request_type);

        BidType request = method.request;
        BidType response = method.response;

        switch (request.type)
        {
        case BID_TYPE_PRIMITIVE:
        case BID_TYPE_ENUM:

            emit_fmt(emit, "req = (typeof(req))req_msg->arg[0];\n", interface->name, method.name);
            break;

        case BID_TYPE_STRUCT:
        {
            int index = 0;
            vec_foreach(member, &request.struct_.members)
            {
                emit_fmt(emit, "req.{} = (typeof(req.{}))req_msg->arg[{}];\n", member.name, member.name, index);
                index++;
            }
        }
        break;

        default:
            break;
        }

        emit_fmt(emit, "{} resp = {{}};\n", response_type);

        PrintTrans err_type = print_trans("{case:pascal}Error", interface->name);

        emit_fmt(emit, "{} error = server->handle_{case:lower}(req_msg->from, &req, &resp, server->ctx);\n", err_type, method.name);

        emit_fmt(emit, "if (error == {case:upper}_SUCCESS)\n", interface->name);
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        emit_fmt(emit, "resp_msg.prot = {case:upper}_PROTOCOL_ID;\n", interface->name);
        emit_fmt(emit, "resp_msg.type = {case:upper}_{case:upper}_RESPONSE;\n", interface->name, method.name);

        switch (response.type)
        {
        case BID_TYPE_PRIMITIVE:
        case BID_TYPE_ENUM:

            emit_fmt(emit, "resp_msg.arg[0] = resp;\n");
            break;

        case BID_TYPE_STRUCT:
        {
            int index = 0;
            vec_foreach(member, &response.struct_.members)
            {
                emit_fmt(emit, "resp_msg.arg[{}] = (BrArg)resp.{};\n", index, member.name);
                index++;
            }
        }
        break;

        default:
            break;
        }

        emit_deident(emit);
        emit_fmt(emit, "}}\n");
        emit_fmt(emit, "else\n");
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        emit_fmt(emit, "resp_msg.prot = {case:upper}_PROTOCOL_ID;\n", interface->name);
        emit_fmt(emit, "resp_msg.type = {case:upper}_ERROR;\n", interface->name);
        emit_fmt(emit, "resp_msg.arg[0] = error;\n");

        emit_deident(emit);
        emit_fmt(emit, "}}\n");

        emit_deident(emit);
        emit_fmt(emit, "}}\n");
        emit_fmt(emit, "break;\n");
        emit_fmt(emit, "\n");
    }

    emit_deident(emit);
    emit_fmt(emit, "}}\n");

    emit_fmt(emit, "\n");
    emit_fmt(emit, "BrResult result = br_ev_resp(req_msg, &resp_msg);\n"
                   "if (result != BR_SUCCESS) {{ server->handle_error(req_msg->from, result, server->ctx); }}\n");

    emit_deident(emit);
    emit_fmt(emit, "}}\n");
}
void bid2c(BidInterface const *interface, Emit *emi, Alloc *alloc)
{

    CUnit unit;
    unit = cunit(alloc);

    cunit_member(&unit, cunit_pragma_once(alloc));
    cunit_member(&unit, cunit_include(true, str$("bal/types"), alloc));
    cunit_member(&unit, cunit_include(true, str$("bal/ev"), alloc));

    // emit_fmt(emit, "#define {case:upper}_PROTOCOL_ID ({#0x})\n\n", interface->name, interface->id);

    cunit_member(&unit, cunit_decl(cdecl_type(interface->name, emit_enum(interface, &interface->errors, alloc), alloc)));

    bid2c_type(&unit, interface, alloc);

    bid2c_methods(&unit, interface, alloc);
}
