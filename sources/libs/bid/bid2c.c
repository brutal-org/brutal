#include <bid/bid.h>

static void emit_type(BidInterface const *interface, BidType const *type, Emit *emit);

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

static void emit_primitive(BidInterface const *interface, BidPrimitive const *type, Emit *emit)
{
    if (type_in_interface(interface, type->name))
    {
        emit_fmt(emit, "{case:pascal}", interface->name);
    }

    emit_fmt(emit, "{}", type->name);
}

static void emit_enum(BidInterface const *interface, BidEnum const *type, Emit *emit, bool error)
{
    emit_fmt(emit, "enum {{\n");
    emit_ident(emit);

    vec_foreach(member, &type->members)
    {
        if (error)
        {
            emit_fmt(emit, "ERR_{case:upper}_{case:upper},\n", interface->name, member);
        }
        else
        {
            emit_fmt(emit, "{case:upper}_{case:upper},\n", interface->name, member);
        }
    }

    emit_deident(emit);
    emit_fmt(emit, "}}");
}

static void emit_struct(BidInterface const *interface, BidStruct const *type, Emit *emit)
{
    emit_fmt(emit, "struct {{\n");
    emit_ident(emit);

    vec_foreach(member, &type->members)
    {
        emit_type(interface, &member.type, emit);
        emit_fmt(emit, " {};\n", member.name);
    }

    emit_deident(emit);
    emit_fmt(emit, "}}");
}

static void emit_type(BidInterface const *interface, BidType const *type, Emit *emit)
{
    switch (type->type)
    {
    case BID_TYPE_PRIMITIVE:
        emit_primitive(interface, &type->primitive_, emit);
        break;

    case BID_TYPE_ENUM:
        emit_enum(interface, &type->enum_, emit, false);
        break;

    case BID_TYPE_STRUCT:
        emit_struct(interface, &type->struct_, emit);
        break;

    default:
        break;
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

static void emit_method_flags(BidType request_type, Emit *emit)
{
    emit_fmt(emit, ".flags = ");

    switch (request_type.type)
    {
    case BID_TYPE_PRIMITIVE:
        if (is_handle(&request_type))
        {
            emit_fmt(emit, "BR_MSG_HND(0),\n");
        }
        else
        {
            emit_fmt(emit, "0,\n");
        }
        break;

    case BID_TYPE_ENUM:
        emit_fmt(emit, "0,\n");
        break;

    case BID_TYPE_STRUCT:
    {

        emit_fmt(emit, "0");
        int index = 0;
        vec_foreach(member, &request_type.struct_.members)
        {
            if (is_handle(&member.type))
            {
                emit_fmt(emit, "| BR_MSG_HND({})", index);
            }
            index++;
        }
        emit_fmt(emit, ",\n");
    }
    break;

    default:
        break;
    }
}
static void emit_msg_args(BidType request_type, Emit *emit)
{
    emit_fmt(emit, ".arg = ");
    emit_fmt(emit, "{{\n");
    emit_ident(emit);

    if (request_type.type != BID_TYPE_NONE)
    {
    }
    switch (request_type.type)
    {
    case BID_TYPE_PRIMITIVE:
    case BID_TYPE_ENUM:
        emit_fmt(emit, "(BrArg)req,\n");
        break;

    case BID_TYPE_STRUCT:
        vec_foreach(member, &request_type.struct_.members)
        {
            emit_fmt(emit, "(BrArg)req->{},\n", member.name);
        }
        break;

    default:
        break;
    }

    emit_deident(emit);
    emit_fmt(emit, "}},\n");
}

static void emit_msg_create(Str interface_name, Str request_name, BidType request_type, Emit *emit)
{
    PrintTrans protocol_id = print_trans("{case:upper}_PROTOCOL_ID", interface_name);
    PrintTrans request = print_trans("{case:upper}_{case:upper}_REQUEST", interface_name, request_name);

    emit_fmt(emit, "BrMsg req_msg = {{\n");
    emit_ident(emit);
    emit_fmt(emit, ".prot = {},\n", protocol_id);
    emit_fmt(emit, ".type = {},\n", request);

    if (request_type.type != BID_TYPE_NONE)
    {
        emit_msg_args(request_type, emit);
        emit_method_flags(request_type, emit);
    }
    emit_deident(emit);
    emit_fmt(emit, "}};\n");
}

static void emit_function(PrintTrans ret, PrintTrans name, PrintTrans *args, int count, Emit *emit)
{
    emit_fmt(emit, "static inline {} {}", ret, name);

    emit_fmt(emit, "(BrId task");

    for (int i = 0; i < count; i++)
    {
        emit_fmt(emit, ", {}", args[i]);
    }
    emit_fmt(emit, ")");
}

/* there are too many arg in this function, maybe put it in a struct ? */
static void emit_method_function(PrintTrans err_type, PrintTrans name_str, PrintTrans request, bool has_request, PrintTrans response, bool has_response, Emit *emit)
{
    PrintTrans func_args[3] = {0};
    int arg_count = 0;

    if (has_request)
    {
        func_args[arg_count] = request;
        arg_count++;
    }
    if (has_response)
    {
        func_args[arg_count] = response;
        arg_count++;
    }

    emit_function(err_type, name_str, func_args, arg_count, emit);
}

static void emit_method(BidInterface const *interface, BidMethod const *method, Emit *emit)
{

    PrintTrans protocol_id = print_trans("{case:upper}_PROTOCOL_ID", interface->name);

    PrintTrans response_id = print_trans(" {case:upper}_{case:upper}_RESPONSE", interface->name, method->name);
    PrintTrans err_type = print_trans(" {case:pascal}Error", interface->name);
    PrintTrans err_id = print_trans(" {case:upper}_ERROR", interface->name);
    PrintTrans request_type = print_trans("{case:pascal}{case:pascal}Request const * req", interface->name, method->name);
    PrintTrans response_type = print_trans("{case:pascal}{case:pascal}Response* resp", interface->name, method->name);
    PrintTrans func_name = print_trans("{case:lower}_{case:lower}", interface->name, method->name);

    emit_method_function(err_type, func_name,
                         request_type, (method->request.type != BID_TYPE_NONE),
                         response_type, (method->response.type != BID_TYPE_NONE),
                         emit);

    emit_fmt(emit, "{{\n");

    emit_ident(emit);

    emit_msg_create(interface->name, method->name, method->request, emit);

    emit_fmt(emit, "BrMsg resp_msg = {{}};\n");
    emit_fmt(emit, "BrResult result = br_ev_req(task, &req_msg, &resp_msg);\n");

    emit_fmt(emit, "if (result != BR_SUCCESS)\n"
                   "{{\n"
                   "return ERR_{case:upper}_BAD_COMMUNICATION;\n"
                   "}}\n",
             interface->name);

    emit_fmt(emit, "if (resp_msg.prot != {} || "
                   "(resp_msg.type != {} && resp_msg.type != {}))\n"
                   "{{ \n"
                   "return ERR_{case:upper}_UNEXPECTED_MESSAGE;\n"
                   "}} \n",
             protocol_id,
             response_id, err_id, interface->name, interface->name);

    emit_fmt(emit, "if (resp_msg.type == {}) \n"
                   "{{\n"
                   "return  ({})resp_msg.arg[0];\n"
                   "}}\n",
             err_id, err_type);

    BidType response = method->response;

    switch (response.type)
    {
    case BID_TYPE_PRIMITIVE:
    case BID_TYPE_ENUM:

        emit_fmt(emit, "*resp = (typeof(*resp))resp_msg.arg[0];", interface->name, method->name);
        break;

    case BID_TYPE_STRUCT:
    {
        int index = 0;
        vec_foreach(member, &response.struct_.members)
        {
            emit_fmt(emit, "resp->{} = (typeof(resp->{}))resp_msg.arg[{}];\n", member.name, member.name, index);
            index++;
        }
    }
    break;

    default:
        break;
    }

    emit_fmt(emit, "return ERR_{case:upper}_SUCCESS;\n",
             interface->name);
    emit_deident(emit);

    emit_fmt(emit, "}}\n");
    emit_fmt(emit, "\n");
}

static void emit_callback(BidInterface const *interface, BidMethod const *method, Emit *emit)
{
    PrintTrans err_type = print_trans("{case:pascal}Error", interface->name);
    PrintTrans request_type = print_trans("{case:pascal}{case:pascal}Request", interface->name, method->name);
    PrintTrans response_type = print_trans("{case:pascal}{case:pascal}Response", interface->name, method->name);
    PrintTrans func_name = print_trans("{case:pascal}{case:pascal}Fn", interface->name, method->name);

    emit_fmt(emit, "typedef {} {}(BrId from, ", err_type, func_name);

    if (method->request.type != BID_TYPE_NONE)
    {
        emit_fmt(emit, "{} const* req, ", request_type);
    }

    if (method->request.type != BID_TYPE_NONE)
    {
        emit_fmt(emit, "{}* res, ", response_type);
    }

    emit_fmt(emit, "void* ctx);\n");
}

void bid2c(BidInterface const *interface, Emit *emit)
{
    emit_fmt(emit, "#pragma once\n"
                   "\n"
                   "// This is file is auto generated by BID, don't edit it!\n"
                   "\n"

                   "#include <syscalls/types.h>\n"
                   "#include <syscalls/ev.h>\n"
                   "\n");

    emit_fmt(emit, "#define {case:upper}_PROTOCOL_ID ({#0x})\n\n", interface->name, interface->id);

    emit_fmt(emit, "typedef ");
    emit_enum(interface, &interface->errors, emit, true);
    emit_fmt(emit, " {case:pascal}Error;\n", interface->name);

    emit_fmt(emit, "\n");

    emit_fmt(emit, "/* --- Types ---------------------------------------------------------------- */\n\n");

    vec_foreach(alias, &interface->aliases)
    {
        emit_fmt(emit, "typedef ");
        emit_type(interface, &alias.type, emit);
        emit_fmt(emit, " {case:pascal}{case:pascal};\n", interface->name, alias.name);
        emit_fmt(emit, "\n");
    }

    emit_fmt(emit, "/* --- Messages ------------------------------------------------------------- */\n\n");

    vec_foreach(method, &interface->methods)
    {
        if (method.request.type != BID_TYPE_NONE)
        {
            emit_fmt(emit, "typedef ");
            emit_type(interface, &method.request, emit);

            emit_fmt(emit, " {case:pascal}{case:pascal}Request;\n", interface->name, method.name);
            emit_fmt(emit, "\n");
        }

        if (method.response.type != BID_TYPE_NONE)
        {
            emit_fmt(emit, "typedef ");
            emit_type(interface, &method.response, emit);
            emit_fmt(emit, " {case:pascal}{case:pascal}Response;\n", interface->name, method.name);
            emit_fmt(emit, "\n");
        }
    }

    vec_foreach(event, &interface->events)
    {
        emit_fmt(emit, "typedef ");
        emit_type(interface, &event.data, emit);
        emit_fmt(emit, " {case:pascal}{case:pascal}Event;\n", interface->name, event.name);
        emit_fmt(emit, "\n");
    }

    emit_fmt(emit, "typedef enum {{\n");

    emit_ident(emit);

    emit_fmt(emit, "{case:upper}_INVALID,\n", interface->name);
    emit_fmt(emit, "{case:upper}_ERROR,\n", interface->name);

    vec_foreach(method, &interface->methods)
    {
        emit_fmt(emit, "{case:upper}_{case:upper}_REQUEST,\n", interface->name, method.name);
        emit_fmt(emit, "{case:upper}_{case:upper}_RESPONSE,\n", interface->name, method.name);
    }

    vec_foreach(event, &interface->events)
    {
        emit_fmt(emit, "{case:upper}_{case:upper}_EVENT,\n", interface->name, event.name);
    }

    emit_deident(emit);

    emit_fmt(emit, "}} {case:pascal}MessageType;\n", interface->name);

    emit_fmt(emit, "\n");

    vec_foreach(method, &interface->methods)
    {
        emit_method(interface, &method, emit);
    }

    vec_foreach(method, &interface->methods)
    {
        emit_callback(interface, &method, emit);
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

        BidType response = method.request;

        switch (response.type)
        {
        case BID_TYPE_PRIMITIVE:
        case BID_TYPE_ENUM:

            emit_fmt(emit, "req = (typeof(req))req_msg->arg[0];\n", interface->name, method.name);
            break;

        case BID_TYPE_STRUCT:
        {
            int index = 0;
            vec_foreach(member, &response.struct_.members)
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

        emit_fmt(emit, "{} error = server->handle_{case:lower}(from, &req, &resp);\n", err_type, method.name);

        emit_deident(emit);
        emit_fmt(emit, "}}\n");
        emit_fmt(emit, "break;\n");
        emit_fmt(emit, "\n");
    }

    emit_deident(emit);
    emit_fmt(emit, "}}\n");

    emit_deident(emit);
    emit_fmt(emit, "}}\n");
}
