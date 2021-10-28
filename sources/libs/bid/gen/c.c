#include <bid/bid.h>
#include <cc/builder.h>
#include <cc/builder/helper.h>
#include <cc/gen.h>

static CType bid2c_type(BidInterface const *interface, BidType const *type, Alloc *alloc);

void bid2c_unit_add_typedef(CUnit *unit, BidInterface const *interface, Str s, BidType type, Alloc *alloc)
{
    CType t = bid2c_type(interface, &type, alloc);
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

static CType bid2c_primitive(BidInterface const *interface, BidPrimitive const *type, Alloc *alloc)
{
    Str res;

    if (type->args.length > 0)
    {
        Str arguments = str_fmt(alloc, "");
        for (int i = 0; i < type->args.length; i++)
        {
            if (i != 0)
            {
                arguments = str_fmt(alloc, "{},", arguments);
            }

            arguments = str_fmt(alloc, "{} {}", arguments, type->args.data[i]);
        }

        res = str_fmt(alloc, "{}({})", type->name, arguments);
    }
    else if (type_in_interface(interface, type->name))
    {
        res = str_fmt(alloc, "{case:pascal}{}", interface->name, type->name);
    }
    else
    {
        res = str_fmt(alloc, "{}", type->name);
    }

    return ctype_name(res, alloc);
}

static CType bid2c_enum(BidInterface const *interface, BidEnum const *type, Alloc *alloc)
{
    CType enum_type = ctype_enum(alloc);

    int value = 0;
    vec_foreach(member, &type->members)
    {

        Str s = str_fmt(alloc, "{case:upper}_{case:upper}", interface->name, member);
        ctype_constant(&enum_type, s, cval_unsigned(value), alloc);

        alloc_free(alloc, s.buffer);
    }
    return enum_type;
}

static CType bid2c_struct(BidInterface const *interface, BidStruct const *type, Alloc *alloc)
{
    CType cstruct = ctype_struct(alloc);
    vec_foreach(member, &type->members)
    {
        ctype_member(&cstruct,
                     member.name,
                     bid2c_type(interface, &member.type, alloc),
                     alloc);
    }

    return cstruct;
}

static CType bid2c_type(BidInterface const *interface, BidType const *type, Alloc *alloc)
{
    switch (type->type)
    {
    case BID_TYPE_PRIMITIVE:
        return bid2c_primitive(interface, &type->primitive_, alloc);

    case BID_TYPE_ENUM:
        return bid2c_enum(interface, &type->enum_, alloc);

    case BID_TYPE_STRUCT:
        return bid2c_struct(interface, &type->struct_, alloc);

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
    {
        if (is_handle(&request_type))
        {
            CExpr call = cexpr_call(alloc, cexpr_ident(str$("BR_MSG_HND"), alloc));
            cexpr_member(&call, cexpr_constant(cval_signed(0)));
            return (call);
        }
        else
        {
            return cexpr_constant(cval_signed(0));
        }
        break;
    }

    case BID_TYPE_ENUM:
    {
        return cexpr_constant(cval_signed(0));
    }

    case BID_TYPE_STRUCT:
    {
        CExpr v = cexpr_constant(cval_signed(0));

        int index = 0;
        vec_foreach(member, &request_type.struct_.members)
        {
            if (is_handle(&member.type) | member.kernel_handle)
            {
                // | BR_MSG_HND(index)
                CExpr call = cexpr_call(alloc, cexpr_ident(str$("BR_MSG_HND"), alloc));
                cexpr_member(&call, cexpr_constant(cval_signed(index)));
                v = cexpr_bit_or(v, call, alloc);
            }
            index++;
        }
        return v;
    }

    default:
        return cexpr_constant(cval_signed(0));
    }
}

static CExpr bid2c_msg_args(BidType request_type, Alloc *alloc)
{
    CExpr arr_init = cexpr_initializer(alloc);
    switch (request_type.type)
    {
    case BID_TYPE_PRIMITIVE:
    case BID_TYPE_ENUM:
    {
        // (BrArg)req
        CExpr targ = cexpr_cast(
            cexpr_ident(str$("req"), alloc),
            ctype_name(str$("BrArg"), alloc),
            alloc);

        cexpr_member(&arr_init, targ);
        break;
    }

    case BID_TYPE_STRUCT:
    {
        vec_foreach(member, &request_type.struct_.members)
        {
            // (BrArg)req->{member.name}
            CExpr targ = cexpr_cast(
                cexpr_ptr_access(
                    cexpr_ident(str$("req"), alloc),
                    cexpr_ident(member.name, alloc),
                    alloc),
                ctype_name(str$("BrArg"), alloc),
                alloc);

            cexpr_member(&arr_init, targ);
        }
        break;
    }

    default:
        return cexpr_empty();
        break;
    }

    return arr_init;
}

static CStmt bid2c_msg_create(BidInterface const *interface, BidMethod const *method, Alloc *alloc)
{
    CExpr msg_init = cexpr_initializer(alloc);

    Str protocol_id = str_fmt(alloc, "{case:upper}_PROTOCOL_ID", interface->name);
    cc_push_initializer_member(&msg_init, str$("prot"), cexpr_ident(protocol_id, alloc), alloc);

    Str request = str_fmt(alloc, "{case:upper}_{case:upper}_REQUEST", interface->name, method->name);
    cc_push_initializer_member(&msg_init, str$("type"), cexpr_ident(request, alloc), alloc);

    if (method->request.type != BID_TYPE_NONE)
    {
        // .arg = {}
        CExpr args = bid2c_msg_args(method->request, alloc);
        cc_push_initializer_member(&msg_init, str$("arg"), args, alloc);

        // .flags = {}
        CExpr flags = bid2c_method_flags(method->request, alloc);
        cc_push_initializer_member(&msg_init, str$("flags"), flags, alloc);
    }
    CStmt res = cc_var_decl_str(str$("BrMsg"), str$("req_msg"), msg_init, alloc);
    return res;
}

static CDecl bid2c_method_decl(BidInterface const *interface, BidMethod const *method, CStmt func_stmt, Alloc *alloc)
{
    Str err_type = str_fmt(alloc, "{case:pascal}Error", interface->name);
    Str func_name = str_fmt(alloc, "{case:lower}_{case:lower}", interface->name, method->name);

    CType ret_type = ctype_name(err_type, alloc);
    CType func_type = ctype_named(ctype_func(ret_type, alloc), func_name, alloc);

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
    cexpr_member(&call, cexpr_ident(str$("task"), alloc));
    cexpr_member(&call, cexpr_ref(cexpr_ident(str$("req_msg"), alloc), alloc));
    cexpr_member(&call, cexpr_ref(cexpr_ident(str$("resp_msg"), alloc), alloc));
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
        cstmt_return(cexpr_ident(bad_communication_str, alloc)), // return bad communication str
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
    Str response_id_str = str_fmt(alloc, "{case:upper}_{case:upper}_RESPONSE", interface->name, method->name);
    Str error_id_str = str_fmt(alloc, "{case:upper}_ERROR", interface->name);
    Str unexpected_str = str_fmt(alloc, "{case:upper}_UNEXPECTED_MESSAGE", interface->name);

    CExpr cexpr_resp_access_type = cc_access_str(str$("resp_msg"), str$("type"), alloc); // resp_msg.type

    CExpr resp_or_error_cond =
        cexpr_or( // ||
            cexpr_not_eq(
                cc_access_str(str$("resp_msg"), str$("prot"), alloc), // resp_msg.prot
                cexpr_ident(protocol_id_str, alloc),
                alloc),
            cexpr_and(
                // resp_msg.type != response_id
                cexpr_not_eq(
                    cexpr_resp_access_type,
                    cexpr_ident(response_id_str, alloc), alloc),
                // resp_msg.type != error_id
                cexpr_not_eq(
                    cexpr_resp_access_type,
                    cexpr_ident(error_id_str, alloc), alloc),
                alloc),
            alloc);

    stmt = cstmt_if(resp_or_error_cond,
                    cstmt_return(cexpr_ident(unexpected_str, alloc)), // return bad communication str
                    alloc);

    cstmt_block_add(targ, stmt);

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
                        cexpr_ident(error_id_str, alloc),
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
            cexpr_deref(cexpr_ident(str$("resp"), alloc), alloc),
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
                           bid2c_type(interface, &member.type, alloc),
                           alloc),
                alloc);

            cstmt_block_add(targ, cstmt_expr(set_expr));
            index++;
        }
        break;
    }

    default:
        break;
    }
}

static CDecl bid2c_method(BidInterface const *interface, BidMethod const *method, Alloc *alloc)
{
    CStmt statement = cstmt_block(alloc);

    cstmt_block_add(&statement, bid2c_msg_create(interface, method, alloc));
    // BrMsg resp_msg = {};
    cstmt_block_add(&statement, cc_var_decl_str(str$("BrMsg"), str$("resp_msg"), cexpr_initializer(alloc), alloc));

    // BrResult result = br_ev_req(task, &req_msg, &resp_msg);
    cstmt_block_add(&statement, cc_var_decl_str(str$("BrResult"), str$("result"), bid2c_br_ev_eq_call(alloc), alloc));

    bid2c_msg_handle(&statement, interface, method, alloc);

    bid2c_msg_resp_init(&statement, interface, method, alloc);

    Str success_str = str_fmt(alloc, "{case:upper}_SUCCESS",
                              interface->name);

    cstmt_block_add(&statement, cstmt_return(cexpr_ident(success_str, alloc))); // return {case:upper}_SUCCESS;

    CDecl func_decl = bid2c_method_decl(interface, method, statement, alloc);
    return func_decl;
}

static CDecl bid2c_callback(BidInterface const *interface, BidMethod const *method, Alloc *alloc)
{
    Str err_type = str_fmt(alloc, "{case:pascal}Error", interface->name);
    Str request_type = str_fmt(alloc, "{case:pascal}{case:pascal}Request", interface->name, method->name);
    Str response_type = str_fmt(alloc, "{case:pascal}{case:pascal}Response", interface->name, method->name);
    Str func_name = str_fmt(alloc, "{case:pascal}{case:pascal}Fn", interface->name, method->name);

    CType ctype_fn = ctype_func(ctype_name(err_type, alloc), alloc);

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
                 ctype_ptr(ctype_void(), alloc), // request_type const *
                 alloc);

    return cdecl_type(str$(func_name), ctype_fn, alloc);
}

/* --- Types ---------------------------------------------------------------- */

void bid2c_typedef(CUnit *unit, const BidInterface *interface, Alloc *alloc)
{
    vec_foreach(alias, &interface->aliases)
    {
        Str s = str_fmt(alloc, "{case:pascal}{case:pascal}", interface->name, alias.name);
        CDecl type_def = cdecl_type(s, bid2c_type(interface, &alias.type, alloc), alloc);
        cunit_member(unit, cunit_decl(type_def));
    }
}

/* --- Messages ------------------------------------------------------------- */

CType bid2c_method_message_type(BidInterface const *interface, Alloc *alloc)
{

    CType message_type_decl = ctype_enum(alloc);

    int index = 0;

#define bid2c_const_msg_type(name)         \
    ctype_constant(&message_type_decl,     \
                   name,                   \
                   cval_unsigned(index++), \
                   alloc);

    bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_INVALID", interface->name));
    bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_ERROR", interface->name));

    vec_foreach(method, &interface->methods)
    {
        bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_{case:upper}_RESPONSE", interface->name, method.name));
        bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_{case:upper}_REQUEST", interface->name, method.name));
    }

    vec_foreach(event, &interface->events)
    {
        bid2c_const_msg_type(str_fmt(alloc, "{case:upper}_{case:upper}_EVENT", interface->name, event.name));
    }

    return message_type_decl;
}

CStmt bid2c_dispatcher_case_if_success(BidInterface const *interface, BidMethod method, Alloc *alloc)
{
    Str response_constant_name = str_fmt(alloc, "{case:upper}_{case:upper}_RESPONSE", interface->name, method.name);
    Str protocol_constant_name = str_fmt(alloc, "{case:upper}_PROTOCOL_ID", interface->name);
    BidType response = method.response;
    CStmt if_true_block = cstmt_block(alloc);

    // resp_msg.prot = {case:upper}_PROTOCOL_ID;
    CExpr set_prot_expr = cexpr_assign(cc_access_str(str$("resp_msg"), str$("prot"), alloc), cexpr_ident(protocol_constant_name, alloc), alloc);
    cstmt_block_add(&if_true_block, cstmt_expr(set_prot_expr));

    // resp_msg.type = {case:upper}_{case:upper}_RESPONSE;
    CExpr set_type_expr = cexpr_assign(cc_access_str(str$("resp_msg"), str$("type"), alloc), cexpr_ident(response_constant_name, alloc), alloc);
    cstmt_block_add(&if_true_block, cstmt_expr(set_type_expr));

    switch (response.type)
    {
    case BID_TYPE_PRIMITIVE:
    case BID_TYPE_ENUM:
    {

        CExpr left_resp = cc_index_constant(cc_access_str(str$("resp_msg"), str$("arg"), alloc), 0, alloc);
        CExpr casted_right = cexpr_cast(cc_access_str(str$("resp"), str$("handle"), alloc), ctype_name(str$("BrId"), alloc), alloc);

        CExpr set_expr = cexpr_assign(left_resp, casted_right, alloc);
        cstmt_block_add(&if_true_block, cstmt_expr(set_expr));
        break;
    }

    case BID_TYPE_STRUCT:
    {
        int index = 0;
        vec_foreach(member, &response.struct_.members)
        {
            // resp_msg.{} = (type)resp.arg[{}];
            CExpr res_msg_expr = cc_access_str(str$("resp"), member.name, alloc);
            CExpr left = cc_index_constant(cc_access_str(str$("resp_msg"), str$("arg"), alloc), index, alloc);
            CExpr res_msg_casted = cexpr_cast(res_msg_expr, ctype_name(str$("BrId"), alloc), alloc);
            CExpr set_expr = cexpr_assign(left, res_msg_casted, alloc);

            cstmt_block_add(&if_true_block, cstmt_expr(set_expr));
            index++;
        }
        break;
    }

    default:
        break;
    }

    return if_true_block;
}

CStmt bid2c_dispatcher_case_if_error(BidInterface const *interface, Alloc *alloc)
{
    Str error_constant_name = str_fmt(alloc, "{case:upper}_ERROR", interface->name);

    Str protocol_constant_name = str_fmt(alloc, "{case:upper}_PROTOCOL_ID", interface->name);
    CStmt if_false_block = cstmt_block(alloc);
    // resp_msg.prot = {case:upper}_PROTOCOL_ID;
    CExpr set_prot_expr = cexpr_assign(cc_access_str(str$("resp_msg"), str$("prot"), alloc), cexpr_ident(protocol_constant_name, alloc), alloc);
    cstmt_block_add(&if_false_block, cstmt_expr(set_prot_expr));

    // resp_msg.type = {case:upper}_{case:upper}_ERROR;
    CExpr set_type_expr = cexpr_assign(cc_access_str(str$("resp_msg"), str$("type"), alloc), cexpr_ident(error_constant_name, alloc), alloc);
    cstmt_block_add(&if_false_block, cstmt_expr(set_type_expr));

    // resp_msg.arg[0] = error;
    CExpr set_error_info = cexpr_assign((cc_index_constant(cc_access_str(str$("resp_msg"), str$("arg"), alloc), 0, alloc)), cexpr_ident(str$("error"), alloc), alloc);
    cstmt_block_add(&if_false_block, cstmt_expr(set_error_info));

    return if_false_block;
}

void bid2c_dispatcher_case(BidInterface const *interface, CStmt *switch_block, BidMethod method, Alloc *alloc)
{
    Str success_constant_name = str_fmt(alloc, "{case:upper}_SUCCESS", interface->name);
    Str request_constant_name = str_fmt(alloc, "{case:upper}_{case:upper}_REQUEST", interface->name, method.name);
    Str request_type_name = str_fmt(alloc, "{case:pascal}{case:pascal}Request", interface->name, method.name);
    Str response_type_name = str_fmt(alloc, "{case:pascal}{case:pascal}Response", interface->name, method.name);
    Str error_type_name = str_fmt(alloc, "{case:pascal}Error", interface->name);
    Str server_handle_name = str_fmt(alloc, "handle_{case:lower}", method.name);
    BidType request = method.request;

    // case {case:upper}_{case:upper}_REQUEST:
    CStmt case_stmt = cstmt_case(cexpr_ident(str$(request_constant_name), alloc));
    CStmt case_block = cstmt_block(alloc);

    // {} req = {}
    cstmt_block_add(&case_block, cc_var_decl_str(request_type_name, str$("req"), cexpr_initializer(alloc), alloc));

    // req = ... req_msg->{}

    switch (request.type)
    {
    case BID_TYPE_PRIMITIVE:
    case BID_TYPE_ENUM:
    {
        // req = (req_type)req_msg->arg[0];
        CExpr req_msg_expr = cc_index_constant(cc_ptr_access_str(str$("req_msg"), str$("arg"), alloc), 0, alloc);

        CExpr req_msg_casted = cexpr_cast(req_msg_expr, ctype_name(request_type_name, alloc), alloc);
        CExpr set_expr = cexpr_assign(cexpr_ident(str$("req"), alloc), req_msg_casted, alloc);
        cstmt_block_add(&case_block, cstmt_expr(set_expr));
        break;
    }

    case BID_TYPE_STRUCT:
    {
        int index = 0;
        vec_foreach(member, &request.struct_.members)
        {
            // req.{} = (type)req_msg->arg[{}];
            CExpr left = cc_access_str(str$("req"), member.name, alloc);
            CExpr req_msg_expr = cc_index_constant(cc_ptr_access_str(str$("req_msg"), str$("arg"), alloc), index, alloc);
            CExpr req_msg_casted = cexpr_cast(req_msg_expr, bid2c_type(interface, &member.type, alloc), alloc);
            CExpr set_expr = cexpr_assign(left, req_msg_casted, alloc);

            cstmt_block_add(&case_block, cstmt_expr(set_expr));
            index++;
        }
    }
    break;

    default:
        break;
    }

    // {} resp = {}
    cstmt_block_add(&case_block, cc_var_decl_str(response_type_name, str$("resp"), cexpr_initializer(alloc), alloc));

    // {} error = server->handle_{case:lower}(req_msg->from, &req, &resp, server->ctx)

    CExpr handle_get = cc_ptr_access_str(str$("server"), str$(server_handle_name), alloc);
    CExpr call = cexpr_call(alloc, handle_get);

    cexpr_member(&call, cc_ptr_access_str(str$("req_msg"), str$("from"), alloc));
    cexpr_member(&call, cexpr_ref(cexpr_ident(str$("req"), alloc), alloc));
    cexpr_member(&call, cexpr_ref(cexpr_ident(str$("resp"), alloc), alloc));
    cexpr_member(&call, cc_ptr_access_str(str$("server"), str$("ctx"), alloc));

    cstmt_block_add(&case_block, cc_var_decl_str(error_type_name, str$("error"), call, alloc));

    // if (error == {case:upper}_SUCCESS)

    // error == {}_SUCCESS
    CExpr check_expr = cexpr_eq(cexpr_ident(str$("error"), alloc), cexpr_ident(success_constant_name, alloc), alloc);

    // if TRUE

    CStmt if_true_block = bid2c_dispatcher_case_if_success(interface, method, alloc);
    CStmt if_false_block = bid2c_dispatcher_case_if_error(interface, alloc);

    CStmt if_statement = cstmt_if_else(check_expr, if_true_block, if_false_block, alloc);

    cstmt_block_add(&case_block, if_statement);

    // break
    cstmt_block_add(&case_block, cstmt_break());
    cstmt_block_add(switch_block, case_stmt);
    cstmt_block_add(switch_block, case_block);
}

CDecl bid2c_dispatcher(BidInterface const *interface, Alloc *alloc)
{
    Str server_handler_name = str_fmt(alloc, "{case:pascal}Server", interface->name);
    Str dispatcher_name = str_fmt(alloc, "{case:lower}_server_dispatch", interface->name);

    // static inline void {case:lower}_server_dispatch({case:pascal}Server* server, BrMsg const* req_msg)
    CType func_type = ctype_func(ctype_void(), alloc);
    ctype_member(&func_type, str$("server"), ctype_ptr(ctype_name(server_handler_name, alloc), alloc), alloc);
    ctype_member(&func_type, str$("req_msg"), ctype_ptr(ctype_attr(ctype_name(str$("BrMsg"), alloc), CTYPE_CONST), alloc), alloc);
    CStmt func_statement = cstmt_block(alloc);

    // BrMsg resp_msg = {}
    cstmt_block_add(&func_statement, cc_var_decl_str(str$("BrMsg"), str$("resp_msg"), cexpr_initializer(alloc), alloc));

    /*
        switch(req_msg->type) {
            case X:
            {
            }
            ...
        }
        */

    // req_msg->type
    CExpr case_expr = cc_ptr_access_str(str$("req_msg"), str$("type"), alloc);
    CStmt switch_block = cstmt_block(alloc);

    vec_foreach(method, &interface->methods)
    {
        bid2c_dispatcher_case(interface, &switch_block, method, alloc);
    }

    CStmt switch_statement = cstmt_switch(case_expr, switch_block, alloc);
    cstmt_block_add(&func_statement, switch_statement);

    // BrResult result = br_ev_resp(req_msg, &resp_msg)
    CExpr br_ev_resp_call = cexpr_call(alloc, cexpr_ident(str$("br_ev_resp"), alloc));

    cexpr_member(&br_ev_resp_call, cexpr_ident(str$("req_msg"), alloc));
    cexpr_member(&br_ev_resp_call, cexpr_ref(cexpr_ident(str$("resp_msg"), alloc), alloc));

    CStmt var_result = cc_var_decl_str(str$("BrResult"), str$("result"), br_ev_resp_call, alloc);
    cstmt_block_add(&func_statement, var_result);

    // if (result != BR_SUCCESS) {{ server->handle_error(req_msg->from, result, server->ctx); }}

    CExpr if_cond = cexpr_str_op(not_eq, str$("result"), str$("BR_SUCCESS"), alloc);
    CExpr server_handle = cexpr_ptr_access(cexpr_ident(str$("server"), alloc), cexpr_ident(str$("handle_error"), alloc), alloc);

    CExpr error_call = cexpr_call(alloc, server_handle);

    cexpr_member(&error_call, cc_ptr_access_str(str$("req_msg"), str$("from"), alloc));
    cexpr_member(&error_call, cexpr_ident(str$("result"), alloc));
    cexpr_member(&error_call, cc_ptr_access_str(str$("server"), str$("ctx"), alloc));

    CStmt if_stmt = cstmt_if(if_cond, cstmt_expr(error_call), alloc);

    cstmt_block_add(&func_statement, if_stmt);

    CDecl decl = cdecl_func(dispatcher_name, func_type, func_statement, alloc);
    decl = cdecl_attrib(decl, CDECL_STATIC | CDECL_INLINE);
    // -------------------------------------------------

    // "if (result != BR_SUCCESS) {{ server->handle_error(req_msg->from, result, server->ctx); }}\n");

    return decl;
}

CDecl bid2c_server_structure_declaration(BidInterface const *interface, Alloc *alloc)
{
    /*
        typedef struct
        {
            void *ctx;
            {}{}Fn *handle_{};
            {}ErrorFn *handle_error;
        } {}Server;
    */

    Str error_fn_name = str_fmt(alloc, "{case:pascal}ErrorFn", interface->name);
    Str server_handler_name = str_fmt(alloc, "{case:pascal}Server", interface->name);

    CType server_handle = ctype_struct(alloc);

    ctype_member(&server_handle, str$("ctx"), ctype_ptr(ctype_void(), alloc), alloc);

    vec_foreach(method, &interface->methods)
    {
        Str func_type_name = str_fmt(alloc, "{case:pascal}{case:pascal}Fn", interface->name, method.name);
        Str member_name = str_fmt(alloc, "handle_{}", method.name);
        CType func_type = ctype_ptr(ctype_name(func_type_name, alloc), alloc);
        ctype_member(&server_handle, member_name, func_type, alloc);
    }

    ctype_member(&server_handle, str$("handle_error"), ctype_ptr(ctype_name(error_fn_name, alloc), alloc), alloc);

    return cdecl_type(server_handler_name, server_handle, alloc);
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
            Str s = str_fmt(alloc, "{case:pascal}{case:pascal}Request", interface->name, method.name);
            bid2c_unit_add_typedef(unit, interface, s, method.request, alloc);
        }

        if (method.response.type != BID_TYPE_NONE)
        {
            Str s = str_fmt(alloc, "{case:pascal}{case:pascal}Response", interface->name, method.name);
            bid2c_unit_add_typedef(unit, interface, s, method.response, alloc);
        }
    }

    vec_foreach(event, &interface->events)
    {
        Str s = str_fmt(alloc, "{case:pascal}{case:pascal}Event", interface->name, event.name);
        bid2c_unit_add_typedef(unit, interface, s, event.data, alloc);
    }

    vec_foreach(method, &interface->methods)
    {
        cunit_member(unit, cunit_decl(bid2c_method(interface, &method, alloc)));
    }

    vec_foreach(method, &interface->methods)
    {
        cunit_member(unit, cunit_decl(bid2c_callback(interface, &method, alloc)));
    }

    Str error_fn_name = str_fmt(alloc, "{case:pascal}ErrorFn", interface->name);

    {
        // typedef void {case:pascal}ErrorFn(BrId from, BrResult error, void* ctx);

        CType t = ctype_func(ctype_void(), alloc);
        ctype_member(&t, str$("from"), ctype_name(str$("BrId"), alloc), alloc);
        ctype_member(&t, str$("error"), ctype_name(str$("BrResult"), alloc), alloc);
        ctype_member(&t, str$("ctx"), ctype_ptr(ctype_void(), alloc), alloc);
        CDecl typedef_decl = cdecl_type(error_fn_name, t, alloc);
        cunit_member(unit, cunit_decl(typedef_decl));
    }

    CDecl typedef_decl = bid2c_server_structure_declaration(interface, alloc);
    cunit_member(unit, cunit_decl(typedef_decl));

    CDecl dispatcher_decl = bid2c_dispatcher(interface, alloc);
    cunit_member(unit, cunit_decl(dispatcher_decl));
}

void bid2c(BidInterface const *interface, Emit *emit, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    cunit_member(&unit, cunit_pragma_once(alloc));
    cunit_member(&unit, cunit_include(true, str$("brutal/types.h"), alloc));
    cunit_member(&unit, cunit_include(true, str$("brutal/ds/vec.h"), alloc));
    cunit_member(&unit, cunit_include(true, str$("bal/types.h"), alloc));
    cunit_member(&unit, cunit_include(true, str$("bal/ev.h"), alloc));

    Str protocol_id = str_fmt(alloc, "{case:upper}_PROTOCOL_ID", interface->name);
    cunit_member(&unit, cunit_define(protocol_id, cexpr_cast(cexpr_constant(cval_unsigned(interface->id)), ctype_name(str$("uint32_t"), alloc), alloc), alloc));

    Str error_type = str_fmt(alloc, "{case:pascal}Error", interface->name);
    cunit_member(&unit, cunit_decl(cdecl_type(error_type, bid2c_enum(interface, &interface->errors, alloc), alloc)));

    bid2c_typedef(&unit, interface, alloc);

    bid2c_methods(&unit, interface, alloc);

    cgen_c_unit(emit, unit);
}
