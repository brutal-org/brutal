#include <brutal/ds/vec.h>
#include <cc/gen.h>

void c_emit_type(Emit *target, CType type);
void c_emit_expr(Emit *target, CExpr expr);
void c_emit_statement(Emit *target, CStmt statement);
/* not all of them are here like a[b] */

void c_emit_op_fix(Emit *target, COp op)
{
    emit_fmt(target, "{}", cop_to_str(op));
}
void c_emit_decl_attribute(Emit *target, CDeclAttr attrib)
{
    if (attrib & CDECL_AUTO)
    {
        emit_fmt(target, "auto ");
    }
    if (attrib & CDECL_STATIC)
    {
        emit_fmt(target, "static ");
    }
    if (attrib & CDECL_REGISTER)
    {
        emit_fmt(target, "register ");
    }
    if (attrib & CDECL_INLINE)
    {
        emit_fmt(target, "inline ");
    }
    if (attrib & CDECL_EXTERN)
    {
        emit_fmt(target, "extern ");
    }
    if (attrib & CDECL_THREAD)
    {
        emit_fmt(target, "thread ");
    }
}

void c_emit_value(Emit *target, CVal value)
{
    switch (value.type)
    {
    case CVAL_SIGNED:
        emit_fmt(target, "{}", value.signed_);
        return;

    case CVAL_UNSIGNED:
        emit_fmt(target, "{}", value.unsigned_);
        return;

    case CVAL_STRING:
        emit_fmt(target, "{}", value.string_);
        return;

    default:
        emit_fmt(target, "");
    }
}

void c_emit_member(Emit *target, CTypeMember type)
{
    c_emit_type(target, type.type);
    emit_fmt(target, " {}", type.name);
}

void c_emit_constant(Emit *target, CTypeConstant member)
{
    emit_fmt(target, "{}", member.name);

    if (member.value.type != CVAL_INVALID)
    {
        emit_fmt(target, " = ");
        c_emit_value(target, member.value);
    }
}
void c_emit_func_type(Emit *target, CType type, Str name)
{
    if (type.func_.ret == nullptr)
    {
        emit_fmt(target, "void ");
    }
    else
    {
        c_emit_type(target, *type.func_.ret);
    }

    emit_fmt(target, "(*{})", name);

    int first = 0;
    vec_foreach(v, &type.func_.params)
    {
        if (first != 0)
        {
            emit_fmt(target, ", ");
        }
        first++;
        c_emit_member(target, v);
    }
}

void c_emit_type(Emit *target, CType type)
{
    emit_fmt(target, "{} ", ctype_to_str(type.type));

    if (type.type == CTYPE_PTR)
    {
        c_emit_type(target, *type.ptr_.subtype);
        emit_fmt(target, "* ");
        return;
    }
    else if (type.type == CTYPE_FUNC)
    {
        c_emit_func_type(target, type, type.name);
        return;
    }
    else if ((type.type == CTYPE_STRUCT || type.type == CTYPE_UNION) && type.struct_.members.length != 0)
    {

        emit_fmt(target, "\n{{\n");
        emit_ident(target);
        vec_foreach(v, &type.struct_.members)
        {
            c_emit_member(target, v);
            emit_fmt(target, ";\n");
        }
        emit_deident(target);
        emit_fmt(target, "\n}}");
    }
    else if (type.type == CTYPE_ENUM)
    {
        emit_fmt(target, "\n{{\n");
        emit_ident(target);
        vec_foreach(v, &type.enum_.constants)
        {
            c_emit_constant(target, v);
            emit_fmt(target, ", \n");
        }
        emit_deident(target);
        emit_fmt(target, "\n}}");
    }
    else if (type.type == CTYPE_ARRAY)
    {
        c_emit_type(target, *type.array_.subtype);
        emit_fmt(target, "{}[{}]", type.name, type.array_.size);
        return;
    }

    emit_fmt(target, "{} ", type.name);
}

void c_emit_expr(Emit *target, CExpr expr)
{

    switch (expr.type)
    {
    case CEXPR_CONSTANT:
        c_emit_value(target, expr.constant_);
        break;
    case CEXPR_IDENTIFIER:
        emit_fmt(target, "{}", expr.identifier_);
        break;
    case CEXPR_PREFIX:
    {
        c_emit_op_fix(target, expr.prefix_.op);
        if (expr.prefix_.expr != NULL)
        {
            c_emit_expr(target, *expr.prefix_.expr);
        }
        break;
    }
    case CEXPR_POSTFIX:
    {
        if (expr.prefix_.expr != NULL)
        {
            c_emit_expr(target, *expr.prefix_.expr);
        }
        c_emit_op_fix(target, expr.prefix_.op);
        break;
    }
    case CEXPR_INFIX:
    {

        c_emit_expr(target, *expr.infix_.lhs);
        if (expr.infix_.op == COP_INDEX)
        {
            emit_fmt(target, "[");
        }

        c_emit_op_fix(target, expr.prefix_.op);
        c_emit_expr(target, *expr.infix_.rhs);
        if (expr.infix_.op == COP_INDEX)
        {
            emit_fmt(target, "]");
        }
        break;
    }
    case CEXPR_CALL:
    {
        c_emit_expr(target, *expr.call_.expr);
        emit_fmt(target, "(");
        bool first = true;
        vec_foreach(v, &expr.call_.args)
        {
            if (first)
            {
                first = !first;
            }
            else
            {
                emit_fmt(target, ", ");
            }

            c_emit_expr(target, v);
        }
        break;
    }
    case CEXPR_CAST:
    {
        emit_fmt(target, " (");

        c_emit_type(target, expr.cast_.type);
        emit_fmt(target, ")");
        c_emit_expr(target, *expr.cast_.expr);
        break;
    }
    case CEXPR_TERNARY:
    {
        c_emit_expr(target, *expr.ternary_.expr_cond);
        emit_fmt(target, " ? ");
        c_emit_expr(target, *expr.ternary_.expr_true);
        emit_fmt(target, " : ");
        c_emit_expr(target, *expr.ternary_.expr_false);
        break;
    }
    default:
        break;
    }
}

void c_emit_decl(Emit *target, CDecl declaration)
{

    c_emit_decl_attribute(target, declaration.attr);

    if (declaration.type == CDECL_TYPE)
    {
        bool is_typedef = declaration.name.len != 0;
        if (is_typedef)
        {
            emit_fmt(target, "typedef ");
        }
        c_emit_type(target, declaration.type_.type);

        if (is_typedef)
        {
            emit_fmt(target, "{}", declaration.name);
        }
    }
    else if (declaration.type == CDECL_VAR)
    {
        c_emit_type(target, declaration.var_.type);
        if (declaration.var_.expr.type != CEXPR_INVALID)
        {
            c_emit_expr(target, declaration.var_.expr);
        }
    }
    else if (declaration.type == CDECL_FUNC)
    {

        c_emit_type(target, declaration.func_.type);

        if (declaration.func_.body.type != CSTMT_INVALID)
        {
            c_emit_statement(target, declaration.func_.body);
        }
    }

    emit_fmt(target, ";\n");
}

bool c_emit_should_statement_endline(CStmtType type)
{
    if (type == CSTMT_BLOCK || type == CSTMT_DEFAULT || type == CSTMT_LABEL || type == CSTMT_CASE)
    {
        return false;
    }

    return true;
}

void c_emit_statement(Emit *target, CStmt statement)
{
    switch (statement.type)
    {

    case CSTMT_DECL:
        c_emit_decl(target, *statement.decl_.decl);
        return;

    case CSTMT_EXPR:
        c_emit_expr(target, statement.expr_.expr);
        return;

    case CSTMT_BLOCK:
    {
        emit_fmt(target, "\n{{\n");
        emit_ident(target);
        vec_foreach(v, &statement.block_.stmts)
        {
            c_emit_statement(target, v);
            if (c_emit_should_statement_endline(v.type))
            {
                emit_fmt(target, ";");
            }
            emit_fmt(target, "\n");
        }
        emit_deident(target);
        emit_fmt(target, "\n}}\n");
        return;
    }
    case CSTMT_IF:
    {
        emit_fmt(target, "if (");
        c_emit_expr(target, statement.if_.expr);
        emit_fmt(target, ") \n");
        c_emit_statement(target, *statement.if_.stmt_true);

        if (statement.if_.stmt_false != nullptr)
        {
            emit_fmt(target, "else ");
            c_emit_statement(target, *statement.if_.stmt_false);
        }

        return;
    }
    case CSTMT_FOR:
    {
        emit_fmt(target, "for (");
        c_emit_statement(target, *statement.for_.init_stmt);

        emit_fmt(target, "; ");
        c_emit_expr(target, statement.for_.cond_expr);

        emit_fmt(target, "; ");
        c_emit_expr(target, statement.for_.iter_expr);

        emit_fmt(target, ")\n");
        c_emit_statement(target, *statement.for_.stmt);
        return;
    }
    case CSTMT_WHILE:
    {
        emit_fmt(target, "while (");
        c_emit_expr(target, statement.while_.expr);
        emit_fmt(target, ") \n");

        c_emit_statement(target, *statement.while_.stmt);
        return;
    }
    case CSTMT_DO:
    {
        emit_fmt(target, "do \n ");

        c_emit_statement(target, *statement.do_.stmt);

        emit_fmt(target, "while (");
        c_emit_expr(target, statement.do_.expr);
        emit_fmt(target, ") \n");
        return;
    }
    case CSTMT_SWITCH:
    {
        emit_fmt(target, "switch (");
        c_emit_expr(target, statement.while_.expr);
        emit_fmt(target, ") \n");
        c_emit_statement(target, *statement.while_.stmt);
        return;
    }

    case CSTMT_RETURN:
    {
        emit_fmt(target, "return ");

        c_emit_expr(target, statement.return_.expr);
        return;
    }

    case CSTMT_GOTO:
        emit_fmt(target, "goto {}", statement.goto_.label);
        return;

    case CSTMT_BREAK:
        emit_fmt(target, "break");
        return;

    case CSTMT_CONTINUE:
        emit_fmt(target, "continue");
        return;

    case CSTMT_LABEL:
        emit_fmt(target, "{}:", statement.label_.label);
        return;

    case CSTMT_CASE:
    {
        emit_fmt(target, "case ");
        c_emit_expr(target, statement.case_.expr);
        emit_fmt(target, ":");
        return;
    }

    case CSTMT_DEFAULT:
        emit_fmt(target, "default:");
        return;

    default:
        return;
    }
}

void c_emit_include(Emit *target, CInclude path)
{
    emit_fmt(target, "#include ");

    if (path.is_q_char)
    {
        emit_fmt(target, "\"{}\"", path.path);
    }
    else
    {
        emit_fmt(target, "<{}>", path.path);
    }
}

void c_generate(Emit *target, CTransUnit unit)
{
    vec_foreach(v, &unit.includes)
    {
        c_emit_include(target, v);
    }

    vec_foreach(v, &unit.declarations)
    {
        c_emit_decl(target, v);
    }
}
