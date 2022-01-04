#include <brutal/debug.h>
#include <json/parser.h>
#include <ud/gen/json.h>
#include "brutal/alloc/base.h"
#include "brutal/ds/vec.h"
#include "json/emit.h"
#include "json/objects.h"
#include "ud/ast/decl.h"
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

void ud_emit_reference(Str reference, Json *json, Alloc *alloc)
{
    (void)alloc;

    json_put(json, str$("reference"), json_str(reference));
}

void ud_emit_type(UdType type, Json *json, Alloc *alloc)
{
    Json jtype = json_object(alloc);

    json_put(&jtype, str$("name"), json_str(str_dup(type.name, alloc)));
    json_put(&jtype, str$("type"), json_str(ud_type_to_str(type.type)));

    json_put(json, str$("type"), jtype);
}

void ud_emit_expr(UdExpr expr, Json *json, Alloc *alloc);

void ud_emit_var_decl(UdDecl decl, Json *json, Alloc *alloc)
{
    Json var = json_object(alloc);

    json_put(&var, str$("name"), json_str(str_dup(decl.name, alloc)));

    ud_emit_type(decl.var.type, &var, alloc);

    json_put(&var, str$("mutable"), decl.var.mutable ? json_true() : json_false());

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

        json_put(&jparam, str$("name"), json_str(str_dup(param->name, alloc)));

        ud_emit_type(param->type, &jparam, alloc);

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

void ud_emit_tuple(UdDecl decl, Json *json, Alloc *alloc)
{
    Json elements = json_array(alloc);

    vec_foreach(field, &decl.type_decl.tuple.fields)
    {
        json_append(&elements, json_str(field->name));
    }

    json_put(json, str$("fields"), elements);
}

void ud_emit_type_decl(UdDecl decl, Json *json, Alloc *alloc)
{
    Json type_decl = json_object(alloc);

    json_put(&type_decl, str$("name"), json_str(decl.name));

    json_put(json, str$("type_decl"), type_decl);

    if (decl.type_decl.type == UD_TYPE_TUPLE)
    {
        ud_emit_tuple(decl, &type_decl, alloc);
    }
}

void ud_emit_decl(UdDecl decl, Json *json, Alloc *alloc)
{
    switch (decl.type)
    {
    case UD_DECL_VAR:
    {
        ud_emit_var_decl(decl, json, alloc);
        break;
    }
    case UD_DECL_FUNC:
    {
        ud_emit_func_decl(decl, json, alloc);
        break;
    }
    case UD_DECL_TYPE:
    {

        ud_emit_type_decl(decl, json, alloc);
        break;
    }
    default:
        break;
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
        ud_emit_reference(expr.reference, json, alloc);
        break;
    }

    case UD_EXPR_FUNC_CALL:
    {
        ud_emit_func_call(expr.func_call, json, alloc);
        break;
    }

    default:
        break;
    }
}

void ud_emit_json(UdAst ast, Emit *emit, Alloc *alloc)
{

    vec_foreach(node, &ast)
    {
        Json json = json_object(alloc);

        ud_emit_expr(node->expr, &json, alloc);

        json_emit(json, emit);
        emit_fmt(emit, "\n");

        map_deinit(&json.object);
    }
}
