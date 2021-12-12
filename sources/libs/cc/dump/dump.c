#include <brutal/debug.h>
#include <cc/dump.h>

void cdump_value(Emit *emit, CVal value)
{
    emit_fmt(emit, "value:{} ", cval_type_to_str(value.type));

    switch (value.type)
    {
    case CVAL_SIGNED:
        emit_fmt(emit, "{}", value.signed_);
        break;

    case CVAL_UNSIGNED:
        emit_fmt(emit, "{}", value.unsigned_);
        break;

    case CVAL_FLOAT:
        emit_fmt(emit, "{}", value.float_);
        break;

    case CVAL_STRING:
        emit_fmt(emit, "\"{}\"", value.string_);
        break;

    default:
        panic$("unknown value type {}", value.type);
    }
}

void cdump_member(Emit *emit, CTypeMembers const *members)
{
    emit_fmt(emit, "members:\n");
    emit_ident(emit);

    vec_foreach_v(member, members)
    {
        emit_fmt(emit, "member:{}\n", member.name);
        emit_ident(emit);
        cdump_type(emit, member.type);
        emit_deident(emit);
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
        if (type.array_.size == CTYPE_ARRAY_UNBOUNDED)
        {
            emit_fmt(emit, "array[]\n");
        }
        else
        {
            emit_fmt(emit, "array[{}]\n", type.array_.size);
        }
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
        cdump_member(emit, &type.struct_.members);
        break;

    case CTYPE_UNION:
        emit_fmt(emit, "union\n");
        cdump_member(emit, &type.union_.members);
        break;

    case CTYPE_ENUM:
        emit_fmt(emit, "enum\n");
        break;

    case CTYPE_FUNC:
        emit_fmt(emit, "func\n");
        emit_fmt(emit, "return:\n");

        emit_ident(emit);
        cdump_type(emit, *type.func_.ret);
        emit_deident(emit);

        cdump_member(emit, &type.func_.params);

        break;

    case CTYPE_NAME:
        emit_fmt(emit, "name\n{}\n", type.name);
        break;

    default:
        panic$("unknown ctype type {}", type.type);
    }

    emit_deident(emit);
}

void cdump_expr(Emit *emit, CExpr expr)
{
    emit_fmt(emit, "expr:{}\n", cexpr_type_to_str(expr.type));
    emit_ident(emit);

    switch (expr.type)
    {
    case CEXPR_EMPTY:
    case CEXPR_SELF:
        break;

    case CEXPR_CONSTANT:
        cdump_value(emit, expr.constant_);
        emit_fmt(emit, "\n");
        break;

    case CEXPR_IDENT:
        emit_fmt(emit, "{}\n", expr.ident_);
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

        vec_foreach_v(v, &expr.call_.args)
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
        emit_fmt(emit, "cond: ");
        cdump_expr(emit, *expr.ternary_.expr_cond);

        emit_fmt(emit, "\ntrue:");
        cdump_expr(emit, *expr.ternary_.expr_true);

        emit_fmt(emit, "\nfalse: ");
        cdump_expr(emit, *expr.ternary_.expr_false);
        break;

    case CEXPR_INITIALIZER:
        vec_foreach_v(v, &expr.initializer_.initializer)
        {
            cdump_expr(emit, v);
        }

        break;

    case CEXPR_LAMBDA:
        emit_fmt(emit, "type: ");
        cdump_type(emit, expr.lambda_.type);

        emit_fmt(emit, "\nbody: ");
        cdump_stmt(emit, *expr.lambda_.body);
        break;

    default:
        panic$("unknown cexpr type {}", expr.type);
    }

    emit_deident(emit);
}

void cdump_stmt(Emit *emit, CStmt stmt)
{
    emit_fmt(emit, "stmt:{}\n", cstmt_type_to_str(stmt.type));
    emit_ident(emit);

    switch (stmt.type)
    {
    case CSTMT_INVALID:
    case CSTMT_EMPTY:
    case CSTMT_CONTINUE:
    case CSTMT_DEFAULT:
    case CSTMT_BREAK:
        break;

    case CSTMT_DECL:
        cdump_decl(emit, *stmt.decl_.decl);
        break;

    case CSTMT_EXPR:
        cdump_expr(emit, stmt.expr_.expr);
        break;

    case CSTMT_BLOCK:
        vec_foreach_v(inner, &stmt.block_.stmts)
        {
            cdump_stmt(emit, inner);
        }
        break;

    case CSTMT_IF:
        cdump_expr(emit, stmt.if_.expr);
        emit_fmt(emit, "true:");
        cdump_stmt(emit, *stmt.if_.stmt_true);
        emit_fmt(emit, "false:");
        cdump_stmt(emit, *stmt.if_.stmt_false);
        break;

    case CSTMT_FOR:
        emit_fmt(emit, "\ninit:");
        cdump_stmt(emit, *stmt.for_.init_stmt);
        emit_fmt(emit, "\ncond:");
        cdump_expr(emit, stmt.for_.cond_expr);
        emit_fmt(emit, "\ninc:");
        cdump_expr(emit, stmt.for_.iter_expr);
        emit_fmt(emit, "\nbody:");
        cdump_stmt(emit, *stmt.for_.stmt);
        emit_fmt(emit, "\n");
        break;

    case CSTMT_WHILE:
        cdump_expr(emit, stmt.while_.expr);
        emit_fmt(emit, "\nbody:");
        cdump_stmt(emit, *stmt.while_.stmt);
        emit_fmt(emit, "\n");
        break;

    case CSTMT_DO:
        cdump_stmt(emit, *stmt.do_.stmt);
        emit_fmt(emit, "\nwhile:");
        cdump_expr(emit, stmt.do_.expr);
        emit_fmt(emit, "\n");
        break;

    case CSTMT_SWITCH:
        cdump_expr(emit, stmt.switch_.expr);
        emit_fmt(emit, "\n");
        cdump_stmt(emit, *stmt.switch_.stmt);
        break;

    case CSTMT_RETURN:
        cdump_expr(emit, stmt.return_.expr);
        break;

    case CSTMT_GOTO:
        emit_fmt(emit, " {}\n", stmt.goto_.label);
        break;

    case CSTMT_LABEL:
        emit_fmt(emit, " {}\n", stmt.label_.label);
        break;

    case CSTMT_CASE:
        cdump_expr(emit, stmt.case_.expr);
        break;

    default:
        panic$("unknown cstmt type {}", stmt.type);
        break;
    }

    emit_deident(emit);
}

void cdump_decl(Emit *emit, CDecl decl)
{
    emit_fmt(emit, " decl:");
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
    else if (decl.type == CDECL_EMPTY)
    {
        emit_fmt(emit, "empty");
    }

    emit_deident(emit);
}

void cdump_unit(Emit *emit, CUnit unit)
{
    emit_fmt(emit, " cunit:\n");
    emit_ident(emit);

    vec_foreach_v(entry, &unit.units)
    {
        switch (entry.type)
        {
        case CUNIT_INCLUDE:
            emit_fmt(emit, " include: \n");
            emit_ident(emit);
            emit_fmt(emit, " path: {}\n", entry._include.path);
            emit_fmt(emit, " system: {}\n", (int)entry._include.is_system);
            emit_deident(emit);
            break;

        case CUNIT_PRAGMA:
            emit_fmt(emit, " pragma: {}\n", entry._pragma.text);
            break;

        case CUNIT_DECLARATION:
            cdump_decl(emit, entry._decl);
            break;

        case CUNIT_DEFINE:
            emit_fmt(emit, " define {}: \n", entry._define.name);
            emit_ident(emit);

            if (entry._define.args.len != 0)
            {
                emit_fmt(emit, " arg: \n");

                vec_foreach_v(arg_name, &entry._define.args)
                {
                    emit_fmt(emit, " - {}\n", arg_name);
                }
            }

            cdump_expr(emit, entry._define.expression);
            emit_deident(emit);
            break;

        default:
            panic$("unknown cunit entry type {}", entry.type);
            break;
        }
    }

    emit_deident(emit);
}
