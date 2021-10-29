#include <cc/gen/dump.h>

void cdump_value(Emit *emit, CVal value)
{
    emit_fmt(emit, "value:{}\n", cval_type_to_str(value.type));
    emit_ident(emit);
    switch (value.type)
    {
    case CVAL_SIGNED:
        emit_fmt(emit, "data: {}\n", value.signed_);
        break;
    case CVAL_UNSIGNED:
        emit_fmt(emit, "data: {}\n", value.unsigned_);
        break;
    case CVAL_FLOAT:
        emit_fmt(emit, "data: (float non supported in fmt)\n");
        break;
    case CVAL_STRING:
        emit_fmt(emit, "data \"{}\"\n", value.string_);
        break;
    default:
        assert_unreachable();
    }
    emit_deident(emit);
}

void cdump_type(Emit *emit, CType type)
{
    emit_fmt(emit, "type:");
    emit_ident(emit);

    switch (type.type)
    {
    case CTYPE_INVALID:
        emit_fmt(emit, "<invalid>\n");
        break;

    case CTYPE_TAIL:
        emit_fmt(emit, "<tail>\n");
        break;

    case CTYPE_ERROR:
        emit_fmt(emit, "<error>\n");
        break;

    case CTYPE_VOID:
        emit_fmt(emit, "void\n");
        break;

    case CTYPE_AUTO:
        emit_fmt(emit, "auto\n");
        break;

    case CTYPE_BOOL:
        emit_fmt(emit, "Bool\n");
        break;

    case CTYPE_PTR:
        emit_fmt(emit, "ptr\n");
        cdump_type(emit, *type.ptr_.subtype);
        break;

    case CTYPE_PARENT:
        emit_fmt(emit, "parent\n");
        cdump_type(emit, *type.parent_.subtype);
        break;

    case CTYPE_ARRAY:
        emit_fmt(emit, "array[{}]\n", type.array_.size);
        cdump_type(emit, *type.array_.subtype);
        break;

    case CTYPE_SIGNED:
        emit_fmt(emit, "signed\n");
        break;

    case CTYPE_UNSIGNED:
        emit_fmt(emit, "unsigned\n");
        break;

    case CTYPE_FLOAT:
        emit_fmt(emit, "float\n");
        break;

    case CTYPE_STRUCT:
        emit_fmt(emit, "struct\n");
        break;

    case CTYPE_UNION:
        emit_fmt(emit, "union\n");
        break;

    case CTYPE_ENUM:
        emit_fmt(emit, "enum\n");
        break;

    case CTYPE_FUNC:
        emit_fmt(emit, "func\n");
        break;

    case CTYPE_NAME:
        emit_fmt(emit, "name\n");
        break;

    default:
        assert_unreachable();
    }

    emit_deident(emit);
}

void cdump_expr(Emit *emit, CExpr expr)
{
    emit_fmt(emit, "expr:{} \n", cexpr_type_to_str(expr.type));
    emit_ident(emit);

    switch (expr.type)
    {
    case CEXPR_EMPTY:
    case CEXPR_SELF:
        break;

    case CEXPR_CONSTANT:
        cdump_value(emit, expr.constant_);
        break;

    case CEXPR_IDENT:
        emit_fmt(emit, "identifier: {}\n", expr.ident_);
        break;

    case CEXPR_POSTFIX:
        emit_fmt(emit, "op: {}\n", cop_to_str(expr.postfix_.op));
        cdump_expr(emit, *expr.postfix_.expr);
        break;

    case CEXPR_PREFIX:
        emit_fmt(emit, "op: {}\n", cop_to_str(expr.postfix_.op));
        cdump_expr(emit, *expr.prefix_.expr);
        break;

    case CEXPR_INFIX:
        emit_fmt(emit, "op: {}\n", cop_to_str(expr.infix_.op));
        cdump_expr(emit, *expr.infix_.lhs);
        cdump_expr(emit, *expr.infix_.rhs);
        break;

    case CEXPR_CALL:
        cdump_expr(emit, *expr.call_.expr);
        emit_fmt(emit, "arg:\n");

        emit_ident(emit);

        vec_foreach(v, &expr.call_.args)
        {
            cdump_expr(emit, v);
        }

        emit_deident(emit);
        break;

    case CEXPR_CAST:
        cdump_type(emit, expr.cast_.type);
        cdump_expr(emit, *expr.cast_.expr);
        break;

    case CEXPR_TERNARY:
        cdump_expr(emit, *expr.ternary_.expr_cond);
        cdump_expr(emit, *expr.ternary_.expr_true);
        cdump_expr(emit, *expr.ternary_.expr_false);
        break;

    case CEXPR_INITIALIZER:
        vec_foreach(v, &expr.initializer_.initializer)
        {
            cdump_expr(emit, v);
        }
        break;

    default:
        assert_unreachable();
    }

    emit_deident(emit);
}

void cdump_stmt(Emit *emit, CStmt stmt)
{
    todo$("dump stmt");
    UNUSED(emit);
    UNUSED(stmt);
}

void cdump_decl(Emit *emit, CDecl decl)
{
    emit_fmt(emit, "decl:");
    emit_ident(emit);
    if (decl.type == CDECL_TYPE)
    {
        emit_fmt(emit, "typedef");
        emit_fmt(emit, " {}\n", decl.name);
        cdump_type(emit, decl.type_.type);
    }
    else if (decl.type == CDECL_VAR)
    {
        emit_fmt(emit, "var");
        emit_fmt(emit, " {}\n", decl.name);
        cdump_type(emit, decl.var_.type);
        cdump_expr(emit, decl.var_.expr);
    }
    else if (decl.type == CDECL_FUNC)
    {
        emit_fmt(emit, "func");
        emit_fmt(emit, " {}\n", decl.name);
        cdump_type(emit, decl.func_.type);
        cdump_stmt(emit, decl.func_.body);
    }
    emit_deident(emit);
}

void cdump_unit(Emit *emit, CUnit unit)
{
    emit_fmt(emit, "cunit\n");
    emit_ident(emit);

    vec_foreach(entry, &unit.units)
    {
        if (entry.type == CUNIT_DECLARATION)
        {
            cdump_decl(emit, entry._decl);
        }
    }

    emit_deident(emit);
}
