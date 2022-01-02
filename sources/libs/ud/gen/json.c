#include <json/parser.h>
#include <ud/gen/json.h>
#include "brutal/ds/vec.h"
#include "json/emit.h"
#include "json/objects.h"
#include "ud/ast/expr.h"
#include "ud/parse/parse.h"

Str ud_str_from_constant(UdVal val, Alloc *alloc)
{
    switch (val.type)
    {
    case UD_VAL_INT:
        return str_fmt(alloc, "{}", val.int_);

    case UD_VAL_STR:
        return val.string;

    default:
        return str$("Unknown");
    }

    return str$("");
}

void ud_emit_constant(UdVal val, Json *json, Alloc *alloc)
{
    json_put(json, str$("constant"), json_str(ud_str_from_constant(val, alloc)));
}

void ud_emit_reference(Str reference, Json *json)
{
    json_put(json, str$("reference"), json_str(reference));
}

void ud_emit_expr(UdExpr expr, Json *json, Alloc *alloc);

void ud_emit_var_decl(UdDecl decl, Json *json, Alloc *alloc)
{
    Json var = json_object(alloc);

    json_put(&var, str$("name"), json_str(decl.name));
    json_put(&var, str$("type"), json_str(decl.var.type.name));

    ud_emit_expr(*decl.var.value, &var, alloc);

    json_put(json, str$("variable"), var);
}

void ud_emit_func_decl(UdDecl decl, Json *json, Alloc *alloc)
{
    Json func = json_object(alloc);

    Json params = json_array(alloc);

    Json body = json_array(alloc);

    json_put(&func, str$("name"), json_str(decl.name));

    json_put(&func, str$("return_type"), json_str(decl.func.return_type.name));

    vec_foreach(param, &decl.func.params)
    {
        Json jparam = json_object(alloc);

        json_put(&jparam, str$("name"), json_str(param->name));
        json_put(&jparam, str$("type"), json_str(param->type.name));

        json_append(&params, jparam);
    }

    vec_foreach(node, &decl.func.body)
    {
        Json jnode = json_object(alloc);

        ud_emit_expr(node->expr, &jnode, alloc);

        json_append(&body, jnode);
    }

    json_put(&func, str$("parameters"), params);

    json_put(&func, str$("body"), body);
    json_put(json, str$("function"), func);
}

void ud_emit_decl(UdDecl decl, Json *json, Alloc *alloc)
{
    if (decl.type == UD_DECL_VAR)
    {
        ud_emit_var_decl(decl, json, alloc);
    }

    else if (decl.type == UD_DECL_FUNC)
    {
        ud_emit_func_decl(decl, json, alloc);
    }
}

void ud_emit_func_call(UdFuncCall call, Json *json, Alloc *alloc)
{
    Json jcall = json_object(alloc);
    Json jparams = json_array(alloc);

    json_put(&jcall, str$("name"), json_str(call.name));

    vec_foreach(param, &call.params)
    {
        Json jparam = json_object(alloc);

        ud_emit_expr(*param, &jparam, alloc);

        json_append(&jparams, jparam);
    }

    json_put(&jcall, str$("parameters"), jparams);

    json_put(json, str$("function_call"), jcall);
}

void ud_emit_expr(UdExpr expr, Json *json, Alloc *alloc)
{

    switch (expr.type)
    {

    case UD_EXPR_DECL:
    {
        ud_emit_decl(expr.decl, json, alloc);
        break;
    }

    case UD_EXPR_CONSTANT:
    {
        ud_emit_constant(expr.const_, json, alloc);
        break;
    }

    case UD_EXPR_REFERENCE:
    {
        ud_emit_reference(expr.reference, json);
        break;
    }

    case UD_EXPR_FUNC_CALL:
    {
        ud_emit_func_call(expr.func_call, json, alloc);
    }

    default:
        break;
    }
}

void ud_emit_json(UdAst ast, Emit *emit, Alloc *alloc)
{
    Json json = json_object(alloc);

    vec_foreach(node, &ast)
    {
        ud_emit_expr(node->expr, &json, alloc);
    }

    json_emit(json, emit);
}
