#include <cc/c2c.h>

/* --- CVal ----------------------------------------------------------------- */

void c2c_value(Emit *emit, CVal value)
{
    switch (value.type)
    {
    case CVAL_SIGNED:
        emit_fmt(emit, "{}", value.signed_);
        break;

    case CVAL_UNSIGNED:
        emit_fmt(emit, "{}", value.unsigned_);
        break;

    case CVAL_STRING:
        emit_fmt(emit, "\"{}\"", value.string_);
        break;

    default:
        emit_fmt(emit, "");
        break;
    }
}

/* --- CType ---------------------------------------------------------------- */

static void c2c_type_start(Emit *emit, CType type);
static void c2c_type_end(Emit *emit, CType type);

static void c2c_member(Emit *emit, CTypeMember type)
{
    c2c_type_start(emit, type.type);
    emit_fmt(emit, " {}", type.name);
    c2c_type_end(emit, type.type);
}

static void c2c_constant(Emit *emit, CTypeConstant member)
{
    emit_fmt(emit, "{}", member.name);

    if (member.value.type != CVAL_INVALID)
    {
        emit_fmt(emit, " = ");
        c2c_value(emit, member.value);
    }
}

static void c2c_type_attr(Emit *emit, CTypeAttr attr)
{
    if (attr & CTYPE_CONST)
    {
        emit_fmt(emit, " const");
    }

    if (attr & CTYPE_RESTRICT)
    {
        emit_fmt(emit, " restrict");
    }

    if (attr & CTYPE_VOLATILE)
    {
        emit_fmt(emit, " volatile");
    }
}

void c2c_type(Emit *emit, CType type)
{
    c2c_type_start(emit, type);
    c2c_type_end(emit, type);
}

static void c2c_type_start(Emit *emit, CType type)
{
    emit_fmt(emit, "{}", ctype_to_str(type.type));

    if (type.type == CTYPE_NAME)
    {
        emit_fmt(emit, type.name);
        c2c_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_PTR)
    {
        c2c_type_start(emit, *type.ptr_.subtype);
        emit_fmt(emit, "*");
        c2c_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_PARENT)
    {
        c2c_type_start(emit, *type.ptr_.subtype);
        emit_fmt(emit, "(");
    }
    else if (type.type == CTYPE_FUNC)
    {
        c2c_type(emit, *type.func_.ret); // return
    }
    else if ((type.type == CTYPE_STRUCT || type.type == CTYPE_UNION) &&
             type.struct_.members.length != 0)
    {
        emit_fmt(emit, "{} ", type.name);

        c2c_type_attr(emit, type.attr);
        emit_fmt(emit, "\n{{\n");
        emit_ident(emit);

        vec_foreach(v, &type.struct_.members)
        {
            c2c_member(emit, v);
            emit_fmt(emit, ";\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");
    }
    else if (type.type == CTYPE_ENUM)
    {
        emit_fmt(emit, "{} ", type.name);
        emit_fmt(emit, "\n{{\n");
        emit_ident(emit);

        vec_foreach(v, &type.enum_.constants)
        {
            c2c_constant(emit, v);
            emit_fmt(emit, ", \n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");

        c2c_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_ARRAY)
    {
        c2c_type_attr(emit, type.attr);
        c2c_type_start(emit, *type.array_.subtype);
    }
}

static void c2c_type_end(Emit *emit, CType type)
{
    if (type.type == CTYPE_PTR)
    {
        c2c_type_end(emit, *type.ptr_.subtype);
    }
    else if (type.type == CTYPE_PARENT)
    {
        emit_fmt(emit, ")");
        c2c_type_end(emit, *type.ptr_.subtype);
    }
    else if (type.type == CTYPE_FUNC)
    {
        emit_fmt(emit, "(");

        bool first = true;
        vec_foreach(v, &type.func_.params)
        {
            if (!first)
            {
                emit_fmt(emit, ", ");
            }
            first = false;
            c2c_member(emit, v);
        }
        emit_fmt(emit, ")");

        c2c_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_ARRAY)
    {
        emit_fmt(emit, "[{}]", type.array_.size);
        c2c_type_end(emit, *type.ptr_.subtype);
    }
}

/* --- CExpr ---------------------------------------------------------------- */

#define C2C_MAX_PRECEDENCE (16)

static void c2c_op_fix(Emit *emit, COp op)
{
    emit_fmt(emit, "{}", cop_to_str(op));
}

static int get_pre(CExpr *expr)
{
    switch (expr->type)
    {
    case CEXPR_PREFIX:
    case CEXPR_POSTFIX:
    case CEXPR_INFIX:
        return cop_pre(expr->infix_.op);

    case CEXPR_IDENT:
    case CEXPR_CONSTANT:
        return 0;

    case CEXPR_CALL:
    case CEXPR_CAST:
    case CEXPR_INITIALIZER:
        return 1;

    case CEXPR_TERNARY:
        return 13;

    default:
        return 0;
    }
}

static void c2c_expr_pre(Emit *emit, CExpr expr, int parent_pre)
{
    int pre = get_pre(&expr);

    if (pre > parent_pre)
    {
        emit_fmt(emit, "(");
    }

    switch (expr.type)
    {
    case CEXPR_CONSTANT:
        c2c_value(emit, expr.constant_);
        break;

    case CEXPR_IDENT:
        emit_fmt(emit, "{}", expr.ident_);
        break;

    case CEXPR_SELF:
        break;

    case CEXPR_PREFIX:
        c2c_op_fix(emit, expr.prefix_.op);

        if (expr.prefix_.expr != nullptr)
        {
            c2c_expr_pre(emit, *expr.prefix_.expr, pre);
        }
        break;

    case CEXPR_POSTFIX:
        if (expr.postfix_.expr != nullptr)
        {
            c2c_expr_pre(emit, *expr.postfix_.expr, pre);
        }
        c2c_op_fix(emit, expr.postfix_.op);
        break;

    case CEXPR_INFIX:
        c2c_expr_pre(emit, *expr.infix_.lhs, pre);
        c2c_op_fix(emit, expr.infix_.op);
        c2c_expr_pre(emit, *expr.infix_.rhs, pre);
        if (expr.infix_.op == COP_INDEX)
        {
            emit_fmt(emit, "]");
        }
        break;

    case CEXPR_CALL:
        c2c_expr_pre(emit, *expr.call_.expr, pre);
        emit_fmt(emit, "(");
        bool first = true;
        vec_foreach(v, &expr.call_.args)
        {
            if (first)
            {
                first = !first;
            }
            else
            {
                emit_fmt(emit, ", ");
            }

            c2c_expr_pre(emit, v, C2C_MAX_PRECEDENCE);
        }
        emit_fmt(emit, ")");
        break;

    case CEXPR_CAST:
        emit_fmt(emit, "(");
        c2c_type(emit, expr.cast_.type);
        emit_fmt(emit, ")");
        c2c_expr_pre(emit, *expr.cast_.expr, pre);
        break;

    case CEXPR_TERNARY:
        c2c_expr_pre(emit, *expr.ternary_.expr_cond, pre);
        emit_fmt(emit, " ? ");
        c2c_expr_pre(emit, *expr.ternary_.expr_true, pre);
        emit_fmt(emit, " : ");
        c2c_expr_pre(emit, *expr.ternary_.expr_false, pre);
        break;

    case CEXPR_INITIALIZER:
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        vec_foreach(v, &expr.initializer_.initializer)
        {
            c2c_expr_pre(emit, v, C2C_MAX_PRECEDENCE);
            emit_fmt(emit, ",\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");
        break;

    default:
        panic$("invalid !");
        break;
    }

    if (pre > parent_pre)
    {
        emit_fmt(emit, ")");
    }
}

void c2c_expr(Emit *emit, CExpr expr)
{
    c2c_expr_pre(emit, expr, C2C_MAX_PRECEDENCE);
}
/* --- CStmt ---------------------------------------------------------------- */

static bool c2c_should_stmt_endline(CStmtType type)
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

void c2c_stmt(Emit *emit, CStmt stmt)
{
    switch (stmt.type)
    {

    case CSTMT_DECL:
        c2c_decl(emit, *stmt.decl_.decl);
        return;

    case CSTMT_EXPR:
        c2c_expr(emit, stmt.expr_.expr);
        return;

    case CSTMT_BLOCK:
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        vec_foreach(v, &stmt.block_.stmts)
        {
            c2c_stmt(emit, v);
            if (c2c_should_stmt_endline(v.type))
            {
                emit_fmt(emit, ";");
            }
            emit_fmt(emit, "\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");
        return;

    case CSTMT_IF:
        emit_fmt(emit, "if (");

        c2c_expr(emit, stmt.if_.expr);
        emit_fmt(emit, ") \n");

        if (stmt.if_.stmt_true->type != CSTMT_BLOCK)
        {
            emit_ident(emit);
            c2c_stmt(emit, *stmt.if_.stmt_true);
            emit_deident(emit);
        }
        else
        {
            c2c_stmt(emit, *stmt.if_.stmt_true);
        }

        if (stmt.if_.stmt_false->type != CSTMT_EMPTY)
        {
            emit_fmt(emit, "else\n");
            if (stmt.if_.stmt_true->type != CSTMT_BLOCK)
            {
                emit_ident(emit);
                c2c_stmt(emit, *stmt.if_.stmt_false);
                emit_deident(emit);
            }
            else
            {
                c2c_stmt(emit, *stmt.if_.stmt_false);
            }
        }
        return;

    case CSTMT_FOR:
        emit_fmt(emit, "for (");
        c2c_stmt(emit, *stmt.for_.init_stmt);

        emit_fmt(emit, "; ");
        c2c_expr(emit, stmt.for_.cond_expr);

        emit_fmt(emit, "; ");
        c2c_expr(emit, stmt.for_.iter_expr);

        emit_fmt(emit, ")\n");
        c2c_stmt(emit, *stmt.for_.stmt);
        return;

    case CSTMT_WHILE:
        emit_fmt(emit, "while (");
        c2c_expr(emit, stmt.while_.expr);
        emit_fmt(emit, ") \n");

        c2c_stmt(emit, *stmt.while_.stmt);
        return;

    case CSTMT_DO:
        emit_fmt(emit, "do \n ");

        c2c_stmt(emit, *stmt.do_.stmt);

        emit_fmt(emit, "while (");
        c2c_expr(emit, stmt.do_.expr);
        emit_fmt(emit, ") \n");
        return;

    case CSTMT_SWITCH:
        emit_fmt(emit, "switch (");
        c2c_expr(emit, stmt.while_.expr);
        emit_fmt(emit, ") \n");
        c2c_stmt(emit, *stmt.while_.stmt);
        return;

    case CSTMT_RETURN:
        emit_fmt(emit, "return ");
        c2c_expr(emit, stmt.return_.expr);
        return;

    case CSTMT_GOTO:
        emit_fmt(emit, "goto {}", stmt.goto_.label);
        return;

    case CSTMT_BREAK:
        emit_fmt(emit, "break");
        return;

    case CSTMT_CONTINUE:
        emit_fmt(emit, "continue");
        return;

    case CSTMT_LABEL:
        emit_fmt(emit, "{}:", stmt.label_.label);
        return;

    case CSTMT_CASE:
        emit_fmt(emit, "case ");
        c2c_expr(emit, stmt.case_.expr);
        emit_fmt(emit, ":");
        return;

    case CSTMT_DEFAULT:
        emit_fmt(emit, "default:");
        return;

    default:
        return;
    }
}

/* --- CDecl ---------------------------------------------------------------- */

static void c2c_decl_attr(Emit *emit, CDeclAttr attr)
{
    if (attr & CDECL_AUTO)
    {
        emit_fmt(emit, "auto ");
    }

    if (attr & CDECL_STATIC)
    {
        emit_fmt(emit, "static ");
    }

    if (attr & CDECL_REGISTER)
    {
        emit_fmt(emit, "register ");
    }

    if (attr & CDECL_INLINE)
    {
        emit_fmt(emit, "inline ");
    }

    if (attr & CDECL_EXTERN)
    {
        emit_fmt(emit, "extern ");
    }

    if (attr & CDECL_THREAD)
    {
        emit_fmt(emit, "thread ");
    }
}

void c2c_decl(Emit *emit, CDecl decl)
{
    c2c_decl_attr(emit, decl.attr);

    if (decl.type == CDECL_TYPE)
    {
        bool is_typedef = decl.name.len != 0;

        if (is_typedef)
        {
            emit_fmt(emit, "typedef ");
        }

        c2c_type_start(emit, decl.type_.type);

        if (is_typedef)
        {
            emit_fmt(emit, " {}", decl.name);
        }

        c2c_type_end(emit, decl.type_.type);
    }
    else if (decl.type == CDECL_VAR)
    {
        c2c_type_start(emit, decl.var_.type);

        emit_fmt(emit, " {} ", decl.name);

        c2c_type_end(emit, decl.var_.type);
        if (decl.var_.expr.type != CEXPR_INVALID && decl.var_.expr.type != CEXPR_EMPTY)
        {
            emit_fmt(emit, "=");
            c2c_expr(emit, decl.var_.expr);
        }
    }
    else if (decl.type == CDECL_FUNC)
    {
        // need to do (int) cast because gcc is not good :(
        assert_equal((int)decl.func_.type.type, (int)CTYPE_FUNC);

        CType func_type = decl.func_.type;

        // return:
        c2c_type(emit, *func_type.func_.ret);

        // name
        emit_fmt(emit, " {}(", decl.name);

        // args
        int first = 0;
        vec_foreach(v, &func_type.func_.params)
        {
            if (first != 0)
            {
                emit_fmt(emit, ", ");
            }
            first++;
            c2c_member(emit, v);
        }

        emit_fmt(emit, ")\n");
        c2c_stmt(emit, decl.func_.body);
    }
}

/* --- CUnit ---------------------------------------------------------------- */

static void c2c_include(Emit *emit, CInclude path)
{
    emit_fmt(emit, "#include ");

    if (path.is_system)
    {
        emit_fmt(emit, "<{}>\n", path.path);
    }
    else
    {
        emit_fmt(emit, "\"{}\"\n", path.path);
    }
}

void c2c_pragma(Emit *emit, CPragma pragma)
{
    emit_fmt(emit, "#pragma ");
    emit_fmt(emit, pragma.text);
    emit_fmt(emit, "\n");
}

void c2c_define(Emit *emit, CDefine define)
{
    emit_fmt(emit, "#define ");
    emit_fmt(emit, define.name);
    if (define.args.length != 0)
    {
        emit_fmt(emit, "(");

        for (int i = 0; i < define.args.length; i++)
        {
            if (i != 0)
            {
                emit_fmt(emit, ", {}", define.args.data[i]);
            }
            else
            {
                emit_fmt(emit, "{}", define.args.data[i]);
            }
        }

        emit_fmt(emit, ")");
    }
    emit_fmt(emit, " ");
    c2c_expr(emit, define.expression);
    emit_fmt(emit, "\n");
}

void c2c_unit(Emit *emit, CUnit unit)
{
    CUnitType prev_type = CUNIT_NONE;

    vec_foreach(entry, &unit.units)
    {
        switch (entry.type)
        {
        case CUNIT_INCLUDE:
            if (prev_type != entry.type)
            {
                emit_fmt(emit, "\n");
            }

            c2c_include(emit, entry._include);
            break;

        case CUNIT_PRAGMA:
            c2c_pragma(emit, entry._pragma);
            break;

        case CUNIT_DECLARATION:
            emit_fmt(emit, "\n");

            c2c_decl(emit, entry._decl);

            if (entry._decl.type != CDECL_FUNC)
            {
                emit_fmt(emit, ";");
            }

            emit_fmt(emit, "\n");
            break;

        case CUNIT_DEFINE:
            if (prev_type != entry.type)
            {
                emit_fmt(emit, "\n");
            }

            c2c_define(emit, entry._define);
            break;

        default:
            break;
        }

        prev_type = entry.type;
    }
}
