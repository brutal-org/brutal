#include <cc/gen.h>

void c2c_type_start(Emit *target, CType type);
void c2c_type_end(Emit *target, CType type);
void c2c_type_abstract(Emit *target, CType type);
void c2c_expr(Emit *target, CExpr expr);
void c2c_stmt(Emit *target, CStmt statement);

void c2c_op_fix(Emit *target, COp op)
{
    emit_fmt(target, "{}", cop_to_str(op));
}

void c2c_decl_attribute(Emit *target, CDeclAttr attrib)
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

void c2c_type_attribute(Emit *target, CTypeAttr attrib)
{
    if (attrib & CTYPE_CONST)
    {
        emit_fmt(target, "const ");
    }

    if (attrib & CTYPE_RESTRICT)
    {
        emit_fmt(target, "restrict ");
    }

    if (attrib & CTYPE_VOLATILE)
    {
        emit_fmt(target, "volatile ");
    }
}
void c2c_value(Emit *target, CVal value)
{
    switch (value.type)
    {
    case CVAL_SIGNED:
        emit_fmt(target, "{}", value.signed_);
        break;

    case CVAL_UNSIGNED:
        emit_fmt(target, "{}", value.unsigned_);
        break;

    case CVAL_STRING:
        emit_fmt(target, "\"{}\"", value.string_);
        break;

    default:
        emit_fmt(target, "");
        break;
    }
}

void c2c_member(Emit *target, CTypeMember type)
{
    c2c_type_start(target, type.type);
    emit_fmt(target, " {}", type.name);
    c2c_type_end(target, type.type);
}

void c2c_func_type(Emit *target, CType type, Str name)
{
    int ptr_count = 0;

    while (type.type == CTYPE_PTR)
    {
        type = *type.ptr_.subtype;
        ptr_count++;
    }

    c2c_type_abstract(target, *type.func_.ret);

    emit_fmt(target, "(");

    for (int i = 0; i < ptr_count; i++)
    {
        emit_fmt(target, "*");
    }

    emit_fmt(target, "{})", name);

    int first = 0;
    vec_foreach(v, &type.func_.params)
    {
        if (first != 0)
        {
            emit_fmt(target, ", ");
        }
        first++;
        c2c_member(target, v);
    }
}

void c2c_constant(Emit *target, CTypeConstant member)
{
    emit_fmt(target, "{}", member.name);
    emit_fmt(target, " = ");
    c2c_value(target, member.value);
}

void c2c_type_start(Emit *target, CType type)
{
    emit_fmt(target, "{} ", ctype_to_str(type.type));

    if (type.type == CTYPE_PTR)
    {

        c2c_type_start(target, *type.ptr_.subtype);
        emit_fmt(target, "* ");
        c2c_type_attribute(target, type.attr);
    }
    else if (type.type == CTYPE_FUNC)
    {
        c2c_type_abstract(target, *type.func_.ret); // return

        emit_fmt(target, "(");

        c2c_type_attribute(target, type.attr);
    }
    else if ((type.type == CTYPE_STRUCT || type.type == CTYPE_UNION) &&
             type.struct_.members.length != 0)
    {
        emit_fmt(target, "{} ", type.name);

        c2c_type_attribute(target, type.attr);
        emit_fmt(target, "\n{{\n");
        emit_ident(target);

        vec_foreach(v, &type.struct_.members)
        {
            c2c_member(target, v);
            emit_fmt(target, ";\n");
        }

        emit_deident(target);
        emit_fmt(target, "\n}}");
    }
    else if (type.type == CTYPE_ENUM)
    {
        c2c_type_attribute(target, type.attr);
        emit_fmt(target, "\n{{\n");
        emit_ident(target);

        vec_foreach(v, &type.enum_.constants)
        {
            c2c_constant(target, v);
            emit_fmt(target, ", \n");
        }

        emit_deident(target);
        emit_fmt(target, "\n}}");

        emit_fmt(target, "{} ", type.name);
    }
    else if (type.type == CTYPE_ARRAY)
    {
        c2c_type_attribute(target, type.attr);
        c2c_type_start(target, *type.array_.subtype);
    }
}

void c2c_type_end(Emit *target, CType type)
{
    if (type.type == CTYPE_PTR)
    {
        c2c_type_end(target, type);
    }
    else if (type.type == CTYPE_FUNC)
    {
        emit_fmt(target, ")(");

        int first = 0;
        vec_foreach(v, &type.func_.params)
        {
            if (first != 0)
            {
                emit_fmt(target, ", ");
            }
            first++;
            c2c_member(target, v);
        }
    }
    else if (type.type == CTYPE_ARRAY)
    {
        emit_fmt(target, "[{}]", type.array_.size);
    }
}

void c2c_abstract_type(Emit *target, CType type)
{
    c2c_type_start(target, type);
    c2c_type_end(target, type);
}

void c2c_expr(Emit *target, CExpr expr)
{
    emit_fmt(target, "(");
    switch (expr.type)
    {
    case CEXPR_CONSTANT:
        c2c_value(target, expr.constant_);
        break;

    case CEXPR_IDENTIFIER:
        emit_fmt(target, "{}", expr.identifier_);
        break;

    case CEXPR_PREFIX:
        c2c_op_fix(target, expr.prefix_.op);
        if (expr.prefix_.expr != NULL)
        {
            c2c_expr(target, *expr.prefix_.expr);
        }
        break;

    case CEXPR_POSTFIX:
        if (expr.prefix_.expr != NULL)
        {
            c2c_expr(target, *expr.prefix_.expr);
        }
        c2c_op_fix(target, expr.prefix_.op);
        break;

    case CEXPR_INFIX:
        c2c_expr(target, *expr.infix_.lhs);
        if (expr.infix_.op == COP_INDEX)
        {
            emit_fmt(target, "[");
        }

        c2c_op_fix(target, expr.prefix_.op);
        c2c_expr(target, *expr.infix_.rhs);

        if (expr.infix_.op == COP_INDEX)
        {
            emit_fmt(target, "]");
        }
        break;

    case CEXPR_CALL:
        c2c_expr(target, *expr.call_.expr);
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

            c2c_expr(target, v);
        }
        break;

    case CEXPR_CAST:
        emit_fmt(target, " (");
        c2c_type_abstract(target, expr.cast_.type);
        emit_fmt(target, ")");
        c2c_expr(target, *expr.cast_.expr);
        break;

    case CEXPR_TERNARY:
        c2c_expr(target, *expr.ternary_.expr_cond);
        emit_fmt(target, " ? ");
        c2c_expr(target, *expr.ternary_.expr_true);
        emit_fmt(target, " : ");
        c2c_expr(target, *expr.ternary_.expr_false);
        break;

    default:
        break;
    }

    emit_fmt(target, ")");
}

void c2c_decl(Emit *target, CDecl declaration)
{
    c2c_decl_attribute(target, declaration.attr);

    if (declaration.type == CDECL_TYPE)
    {
        bool is_typedef = declaration.name.len != 0;

        if (is_typedef)
        {
            emit_fmt(target, "typedef ");
        }

        c2c_type_start(target, declaration.type_.type);

        if (is_typedef)
        {
            emit_fmt(target, "{}", declaration.name);
        }

        c2c_type_end(target, declaration.type_.type);
    }
    else if (declaration.type == CDECL_VAR)
    {
        c2c_type_start(target, declaration.var_.type);

        emit_fmt(target, "{}", declaration.name);

        c2c_type_end(target, declaration.var_.type);
        if (declaration.var_.expr.type != CEXPR_INVALID)
        {
            c2c_expr(target, declaration.var_.expr);
        }
    }
    else if (declaration.type == CDECL_FUNC)
    {
        // need to do (int) cast because gcc is not good :(
        assert_equal((int)declaration.func_.type.type, (int)CTYPE_FUNC);

        auto func_type = declaration.func_.type;

        // return:
        c2c_type_abstract(target, *func_type.func_.ret);

        // name
        emit_fmt(target, " {}", declaration.name);

        // args
        int first = 0;
        vec_foreach(v, &func_type.func_.params)
        {
            if (first != 0)
            {
                emit_fmt(target, ", ");
            }
            first++;
            c2c_member(target, v);
        }

        c2c_stmt(target, declaration.func_.body);
    }

    emit_fmt(target, ";\n");
}

bool c2c_should_statement_endline(CStmtType type)
{
    if (type == CSTMT_BLOCK ||
        type == CSTMT_DEFAULT ||
        type == CSTMT_LABEL ||
        type == CSTMT_CASE)
    {
        return false;
    }

    return true;
}

void c2c_stmt(Emit *target, CStmt statement)
{
    switch (statement.type)
    {

    case CSTMT_DECL:
        c2c_decl(target, *statement.decl_.decl);
        return;

    case CSTMT_EXPR:
        c2c_expr(target, statement.expr_.expr);
        return;

    case CSTMT_BLOCK:
        emit_fmt(target, "\n{{\n");
        emit_ident(target);

        vec_foreach(v, &statement.block_.stmts)
        {
            c2c_stmt(target, v);
            if (c2c_should_statement_endline(v.type))
            {
                emit_fmt(target, ";");
            }
            emit_fmt(target, "\n");
        }

        emit_deident(target);
        emit_fmt(target, "\n}}\n");
        return;

    case CSTMT_IF:
        emit_fmt(target, "if (");
        c2c_expr(target, statement.if_.expr);
        emit_fmt(target, ") \n");
        c2c_stmt(target, *statement.if_.stmt_true);

        if (statement.if_.stmt_false->type != CSTMT_EMPTY)
        {
            emit_fmt(target, "else ");
            c2c_stmt(target, *statement.if_.stmt_false);
        }
        return;

    case CSTMT_FOR:
        emit_fmt(target, "for (");
        c2c_stmt(target, *statement.for_.init_stmt);

        emit_fmt(target, "; ");
        c2c_expr(target, statement.for_.cond_expr);

        emit_fmt(target, "; ");
        c2c_expr(target, statement.for_.iter_expr);

        emit_fmt(target, ")\n");
        c2c_stmt(target, *statement.for_.stmt);
        return;

    case CSTMT_WHILE:
        emit_fmt(target, "while (");
        c2c_expr(target, statement.while_.expr);
        emit_fmt(target, ") \n");

        c2c_stmt(target, *statement.while_.stmt);
        return;

    case CSTMT_DO:
        emit_fmt(target, "do \n ");

        c2c_stmt(target, *statement.do_.stmt);

        emit_fmt(target, "while (");
        c2c_expr(target, statement.do_.expr);
        emit_fmt(target, ") \n");
        return;

    case CSTMT_SWITCH:
        emit_fmt(target, "switch (");
        c2c_expr(target, statement.while_.expr);
        emit_fmt(target, ") \n");
        c2c_stmt(target, *statement.while_.stmt);
        return;

    case CSTMT_RETURN:
        emit_fmt(target, "return ");
        c2c_expr(target, statement.return_.expr);
        return;

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
        emit_fmt(target, "case ");
        c2c_expr(target, statement.case_.expr);
        emit_fmt(target, ":");
        return;

    case CSTMT_DEFAULT:
        emit_fmt(target, "default:");
        return;

    default:
        return;
    }
}

void c2c_include(Emit *target, CInclude path)
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

void c2c(Emit *target, CTransUnit unit)
{
    vec_foreach(v, &unit.includes)
    {
        c2c_include(target, v);
    }

    vec_foreach(v, &unit.declarations)
    {
        c2c_decl(target, v);
    }
}
