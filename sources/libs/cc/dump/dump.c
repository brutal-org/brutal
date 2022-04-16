#include <cc/dump.h>
#include <json/objects.h>
#include <brutal-debug>

Json cdump_value(CVal value, Alloc *alloc)
{
    Json json = json_object(alloc);
    json_put(&json, str$("type"), json_str(cval_type_to_str(value.type)));

    switch (value.type)
    {
    case CVAL_SIGNED:
    {
        json_put(&json, str$("value"), json_number(value.signed_));
        break;
    }
    case CVAL_UNSIGNED:
    {
        json_put(&json, str$("value"), json_number(value.unsigned_));
        break;
    }
    case CVAL_FLOAT:
    {
        json_put(&json, str$("value"), json_str(str$("float are not supported for the moment")));
        break;
    }
    case CVAL_STRING:
    {
        json_put(&json, str$("value"), json_str(value.string_));
        break;
    }
    default:
        panic$("unknown value type {}", value.type);
    }
    return json;
}

Json cdump_member(CTypeMembers const *members, Alloc *alloc)
{
    Json json = json_array(alloc);

    vec_foreach_v(member, members)
    {
        Json member_json = json_object(alloc);
        json_put(&member_json, str$("name"), json_str(member.name));
        json_put(&member_json, str$("type"), cdump_type(member.type, alloc));
    }
    return json;
}

Json cdump_type(CType type, Alloc *alloc)
{
    switch (type.type)
    {
    case CTYPE_INVALID:
    {
        return json_str(str$("<invalid>"));
    }
    case CTYPE_TAIL:
    {
        return json_str(str$("<tail>"));
    }
    case CTYPE_ERROR:
    {
        return json_str(str$("<error>"));
    }
    case CTYPE_VOID:
    {
        return json_str(str$("void"));
    }
    case CTYPE_AUTO:
    {
        return json_str(str$("auto"));
    }
    case CTYPE_BOOL:
    {
        return json_str(str$("Bool"));
    }
    case CTYPE_PTR:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("ptr"), cdump_type(*type.ptr_.subtype, alloc));
        return json;
    }
    case CTYPE_PARENT:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("parent"), cdump_type(*type.parent_.subtype, alloc));
        return json;
    }
    case CTYPE_ARRAY:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("array"), cdump_type(*type.array_.subtype, alloc));
        if (type.array_.size != CTYPE_ARRAY_UNBOUNDED)
        {
            json_put(&json, str$("size"), json_number(type.array_.size));
        }
        return json;
    }
    case CTYPE_SIGNED:
    {
        return json_str(str$("signed"));
    }
    case CTYPE_UNSIGNED:
    {
        return json_str(str$("unsigned"));
    }
    case CTYPE_FLOAT:
    {
        return json_str(str$("float"));
    }
    case CTYPE_STRUCT:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("struct"), cdump_member(&type.struct_.members, alloc));
        return json;
    }
    case CTYPE_UNION:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("union"), cdump_member(&type.struct_.members, alloc));
        return json;
    }
    case CTYPE_ENUM:
    {
        return json_str(str$("enum"));
    }
    case CTYPE_FUNC:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("func"), cdump_type(*type.func_.ret, alloc));
        json_put(&json, str$("args"), cdump_member(&type.func_.params, alloc));
        return json;
    }
    case CTYPE_NAME:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("name"), json_str(type.name));
        return json;
    }
    default:
        panic$("unknown ctype type {}", type.type);
    }
}

Json cdump_expr(CExpr expr, Alloc *alloc)
{
    Json json = json_object(alloc);
    if (expr.sema_type.type != CTYPE_INVALID)
    {
        json_put(&json, str$("sema-type"), cdump_type(expr.sema_type, alloc));
    }

    switch (expr.type)
    {
    case CEXPR_EMPTY:
    case CEXPR_SELF:
    {
        break;
    }
    case CEXPR_CONSTANT:
    {
        json_put(&json, str$("constant"), cdump_value(expr.constant_, alloc));
        break;
    }
    case CEXPR_IDENT:
    {
        json_put(&json, str$("ident"), json_str(expr.ident_));
        break;
    }
    case CEXPR_POSTFIX:
    {
        json_put(&json, str$("postfix"), json_str(cop_to_str(expr.postfix_.op)));
        json_put(&json, str$("expr"), cdump_expr(*expr.postfix_.expr, alloc));
        break;
    }

    case CEXPR_PREFIX:
    {
        json_put(&json, str$("prefix"), json_str(cop_to_str(expr.prefix_.op)));
        json_put(&json, str$("expr"), cdump_expr(*expr.prefix_.expr, alloc));
        break;
    }
    case CEXPR_INFIX:
    {
        json_put(&json, str$("infix"), json_str(cop_to_str(expr.infix_.op)));
        json_put(&json, str$("lhs-expr"), cdump_expr(*expr.infix_.lhs, alloc));
        json_put(&json, str$("rhs-expr"), cdump_expr(*expr.infix_.rhs, alloc));
        break;
    }
    case CEXPR_CALL:
    {
        json_put(&json, str$("call"), cdump_expr(*expr.call_.expr, alloc));
        Json args = json_array(alloc);

        vec_foreach_v(v, &expr.call_.args)
        {
            json_append(&args, cdump_expr(v, alloc));
        }

        json_put(&json, str$("args"), args);
        break;
    }
    case CEXPR_CAST:
    {
        json_put(&json, str$("cast"), cdump_type(expr.cast_.type, alloc));
        json_put(&json, str$("expr"), cdump_expr(*expr.cast_.expr, alloc));
        break;
    }
    case CEXPR_TERNARY:
    {
        json_put(&json, str$("ternary"), cdump_expr(*expr.ternary_.expr_cond, alloc));
        json_put(&json, str$("true"), cdump_expr(*expr.ternary_.expr_true, alloc));
        json_put(&json, str$("false"), cdump_expr(*expr.ternary_.expr_false, alloc));
        break;
    }
    case CEXPR_INITIALIZER:
    {
        Json members = json_array(alloc);
        vec_foreach_v(v, &expr.initializer_.initializer)
        {
            json_append(&members, cdump_expr(v, alloc));
        }
        json_put(&json, str$("initializer"), members);
        break;
    }
    case CEXPR_LAMBDA:
    {
        json_put(&json, str$("lambda"), cdump_type(expr.lambda_.type, alloc));
        json_put(&json, str$("body"), cdump_stmt(*expr.lambda_.body, alloc));
        break;
    }
    default:
        panic$("unknown cexpr type {}", expr.type);
    }
    return json;
}

Json cdump_stmt(CStmt stmt, Alloc *alloc)
{
    switch (stmt.type)
    {
    case CSTMT_INVALID:
    case CSTMT_EMPTY:
    case CSTMT_CONTINUE:
    case CSTMT_DEFAULT:
    case CSTMT_BREAK:
    {
        return json_str(cstmt_type_to_str(stmt.type));
    }
    case CSTMT_DECL:
    {
        return cdump_decl(*stmt.decl_.decl, alloc);
    }
    case CSTMT_EXPR:
    {
        return cdump_expr(stmt.expr_.expr, alloc);
    }
    case CSTMT_BLOCK:
    {
        Json array = json_array(alloc);
        vec_foreach_v(inner, &stmt.block_.stmts)
        {
            json_append(&array, cdump_stmt(inner, alloc));
        }
        return array;
    }
    case CSTMT_IF:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("if"), cdump_expr(stmt.if_.expr, alloc));
        json_put(&json, str$("true"), cdump_stmt(*stmt.if_.stmt_true, alloc));
        json_put(&json, str$("false"), cdump_stmt(*stmt.if_.stmt_false, alloc));
        return json;
    }
    case CSTMT_FOR:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("for"), cdump_stmt(*stmt.for_.init_stmt, alloc));
        json_put(&json, str$("cond"), cdump_expr(stmt.for_.cond_expr, alloc));
        json_put(&json, str$("inc"), cdump_expr(stmt.for_.iter_expr, alloc));
        json_put(&json, str$("body"), cdump_stmt(*stmt.for_.stmt, alloc));
        return json;
    }
    case CSTMT_WHILE:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("while"), cdump_expr(stmt.while_.expr, alloc));
        json_put(&json, str$("body"), cdump_stmt(*stmt.while_.stmt, alloc));
        return json;
    }
    case CSTMT_DO:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("do-while"), cdump_expr(stmt.do_.expr, alloc));
        json_put(&json, str$("body"), cdump_stmt(*stmt.do_.stmt, alloc));
        return json;
    }
    case CSTMT_SWITCH:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("switch"), cdump_expr(stmt.switch_.expr, alloc));
        json_put(&json, str$("body"), cdump_stmt(*stmt.switch_.stmt, alloc));
        return json;
    }
    case CSTMT_RETURN:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("return"), cdump_expr(stmt.return_.expr, alloc));
        return json;
    }
    case CSTMT_GOTO:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("goto"), json_str(stmt.goto_.label));
        return json;
    }
    case CSTMT_LABEL:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("label"), json_str(stmt.goto_.label));
        return json;
    }
    case CSTMT_CASE:
    {
        Json json = json_object(alloc);
        json_put(&json, str$("case"), cdump_expr(stmt.case_.expr, alloc));
        return json;
    }
    default:
    {
        panic$("unknown cstmt type {}", stmt.type);
        break;
    }
    }
}

Json cdump_decl(CDecl decl, Alloc *alloc)
{
    Json json = json_object(alloc);
    if (decl.sema_type.type != CTYPE_INVALID)
    {
        json_put(&json, str$("sema-type"), cdump_type(decl.sema_type, alloc));
    }

    if (decl.type == CDECL_TYPE)
    {
        json_put(&json, str$("typedef"), json_str(decl.name));
        json_put(&json, str$("type"), cdump_type(decl.type_.type, alloc));
    }
    else if (decl.type == CDECL_VAR)
    {
        json_put(&json, str$("var"), json_str(decl.name));
        json_put(&json, str$("type"), cdump_type(decl.var_.type, alloc));
        json_put(&json, str$("expr"), cdump_expr(decl.var_.expr, alloc));
    }
    else if (decl.type == CDECL_FUNC)
    {
        json_put(&json, str$("func"), json_str(decl.name));
        json_put(&json, str$("type"), cdump_type(decl.func_.type, alloc));
        json_put(&json, str$("body"), cdump_stmt(decl.func_.body, alloc));
    }
    else if (decl.type == CDECL_EMPTY)
    {
        json_put(&json, str$("empty"), json_str(str$("empty")));
    }
    else
    {
        panic$("unknown cdecl type {}", decl.type);
    }
    return json;
}

Json cdump_unit(CUnit unit, Alloc *alloc)
{
    Json json = json_object(alloc);
    /* todo: add file info and more here */
    Json units = json_array(alloc);
    vec_foreach_v(entry, &unit.units)
    {
        switch (entry.type)
        {
        case CUNIT_INCLUDE:
        {
            Json include = json_object(alloc);
            json_put(&include, str$("include"), json_str(entry._include.path));
            json_put(&include, str$("system"), json_bool(entry._include.is_system));
            json_append(&units, include);
            break;
        }
        case CUNIT_PRAGMA:
        {
            Json pragma = json_object(alloc);
            json_put(&pragma, str$("pragma"), json_str(entry._pragma.text));
            json_append(&units, pragma);
            break;
        }
        case CUNIT_DECLARATION:
        {
            json_append(&units, cdump_decl(entry._decl, alloc));
            break;
        }
        case CUNIT_DEFINE:
        {
            Json def = json_object(alloc);
            json_put(&def, str$("define"), json_str(entry._define.name));

            if (entry._define.args.len != 0)
            {
                Json args = json_array(alloc);

                vec_foreach_v(arg_name, &entry._define.args)
                {
                    json_append(&args, json_str(arg_name));
                }
                json_put(&def, str$("args"), args);
            }
            json_put(&def, str$("expr"), cdump_expr(entry._define.expression, alloc));
            json_append(&units, def);
            break;
        }

        default:
            panic$("unknown cunit entry type {}", entry.type);
            break;
        }
    }
    json_put(&json, str$("unit"), units);
    return json;
}
