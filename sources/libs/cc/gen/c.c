#include <brutal/debug.h>
#include <cc/gen/c.h>

/* --- CVal ----------------------------------------------------------------- */

void cgen_c_value(Emit *emit, CVal value)
{
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
        panic$("Unknown CVal type {}", value.type);
        break;
    }
}

/* --- CType ---------------------------------------------------------------- */

static void cgen_c_type_start(Emit *emit, CType type);
static void cgen_c_type_end(Emit *emit, CType type);

static void cgen_c_member(Emit *emit, CTypeMember type)
{
    cgen_c_type_start(emit, type.type);
    emit_fmt(emit, " {}", type.name);
    cgen_c_type_end(emit, type.type);
}

static void cgen_c_constant(Emit *emit, CTypeConstant member)
{
    emit_fmt(emit, "{}", member.name);

    if (member.value.type != CVAL_INVALID)
    {
        emit_fmt(emit, " = ");
        cgen_c_value(emit, member.value);
    }
}

static void cgen_c_type_attr(Emit *emit, CTypeAttr attr)
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

void cgen_c_type(Emit *emit, CType type)
{
    cgen_c_type_start(emit, type);
    cgen_c_type_end(emit, type);
}

static void cgen_c_type_start(Emit *emit, CType type)
{
    if (type.type == CTYPE_NAME)
    {
        emit_fmt(emit, type.name);
        cgen_c_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_PTR)
    {
        cgen_c_type_start(emit, *type.ptr_.subtype);
        emit_fmt(emit, "*");
        cgen_c_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_PARENT)
    {
        cgen_c_type_start(emit, *type.ptr_.subtype);
        emit_fmt(emit, "(");
    }
    else if (type.type == CTYPE_FUNC)
    {
        cgen_c_type(emit, *type.func_.ret); // return
    }
    else if ((type.type == CTYPE_STRUCT || type.type == CTYPE_UNION) &&
             type.struct_.members.length != 0)
    {
        if (type.type == CTYPE_STRUCT)
        {
            emit_fmt(emit, "struct");
        }
        else
        {
            emit_fmt(emit, "union");
        }

        emit_fmt(emit, "{} ", type.name);

        cgen_c_type_attr(emit, type.attr);
        emit_fmt(emit, "\n{{\n");
        emit_ident(emit);

        vec_foreach(v, &type.struct_.members)
        {
            cgen_c_member(emit, v);
            emit_fmt(emit, ";\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");
    }
    else if (type.type == CTYPE_ENUM)
    {
        emit_fmt(emit, "enum {} ", type.name);
        emit_fmt(emit, "\n{{\n");
        emit_ident(emit);

        vec_foreach(v, &type.enum_.constants)
        {
            cgen_c_constant(emit, v);
            emit_fmt(emit, ", \n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");

        cgen_c_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_ARRAY)
    {
        cgen_c_type_attr(emit, type.attr);
        cgen_c_type_start(emit, *type.array_.subtype);
    }
    else
    {
        emit_fmt(emit, ctype_to_str(type.type));
    }
}

static void cgen_c_type_end(Emit *emit, CType type)
{
    if (type.type == CTYPE_PTR)
    {
        cgen_c_type_end(emit, *type.ptr_.subtype);
    }
    else if (type.type == CTYPE_PARENT)
    {
        emit_fmt(emit, ")");
        cgen_c_type_end(emit, *type.ptr_.subtype);
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
            cgen_c_member(emit, v);
        }
        emit_fmt(emit, ")");

        cgen_c_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_ARRAY)
    {
        if (type.array_.size == CTYPE_ARRAY_UNBOUNDED)
        {
            emit_fmt(emit, "[]", type.array_.size);
        }
        else
        {
            emit_fmt(emit, "[{}]", type.array_.size);
        }
        cgen_c_type_end(emit, *type.ptr_.subtype);
    }
}

/* --- CExpr ---------------------------------------------------------------- */

static void cgen_c_op_fix(Emit *emit, COp op)
{
    emit_fmt(emit, "{}", cop_to_str(op));
}

static void cgen_c_expr_pre(Emit *emit, CExpr expr, int parent_pre)
{
    int pre = cexpr_pre(&expr);

    if (pre > parent_pre)
    {
        emit_fmt(emit, "(");
    }

    switch (expr.type)
    {
    case CEXPR_EMPTY:
        break;

    case CEXPR_CONSTANT:
        cgen_c_value(emit, expr.constant_);
        break;

    case CEXPR_IDENT:
        emit_fmt(emit, "{}", expr.ident_);
        break;

    case CEXPR_SELF:
        break;

    case CEXPR_PREFIX:
        cgen_c_op_fix(emit, expr.prefix_.op);

        if (expr.prefix_.expr != nullptr)
        {
            cgen_c_expr_pre(emit, *expr.prefix_.expr, pre);
        }
        break;

    case CEXPR_POSTFIX:
        if (expr.postfix_.expr != nullptr)
        {
            cgen_c_expr_pre(emit, *expr.postfix_.expr, pre);
        }
        cgen_c_op_fix(emit, expr.postfix_.op);
        break;

    case CEXPR_INFIX:
        cgen_c_expr_pre(emit, *expr.infix_.lhs, pre);
        cgen_c_op_fix(emit, expr.infix_.op);
        cgen_c_expr_pre(emit, *expr.infix_.rhs, pre);
        if (expr.infix_.op == COP_INDEX)
        {
            emit_fmt(emit, "]");
        }
        break;

    case CEXPR_CALL:
        cgen_c_expr_pre(emit, *expr.call_.expr, pre);
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

            cgen_c_expr_pre(emit, v, CEXPR_MAX_PRECEDENCE);
        }
        emit_fmt(emit, ")");
        break;

    case CEXPR_CAST:
        emit_fmt(emit, "(");
        cgen_c_type(emit, expr.cast_.type);
        emit_fmt(emit, ")");
        cgen_c_expr_pre(emit, *expr.cast_.expr, pre);
        break;

    case CEXPR_TERNARY:
        cgen_c_expr_pre(emit, *expr.ternary_.expr_cond, pre);
        emit_fmt(emit, " ? ");
        cgen_c_expr_pre(emit, *expr.ternary_.expr_true, pre);
        emit_fmt(emit, " : ");
        cgen_c_expr_pre(emit, *expr.ternary_.expr_false, pre);
        break;

    case CEXPR_INITIALIZER:
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        vec_foreach(v, &expr.initializer_.initializer)
        {
            cgen_c_expr_pre(emit, v, CEXPR_MAX_PRECEDENCE);
            emit_fmt(emit, ",\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");
        break;

    default:
        panic$("Unknown cexpr type {}", expr.type);
    }

    if (pre > parent_pre)
    {
        emit_fmt(emit, ")");
    }
}

void cgen_c_expr(Emit *emit, CExpr expr)
{
    cgen_c_expr_pre(emit, expr, CEXPR_MAX_PRECEDENCE);
}
/* --- CStmt ---------------------------------------------------------------- */

static bool cgen_c_should_stmt_endline(CStmtType type)
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

void cgen_c_stmt(Emit *emit, CStmt stmt)
{
    switch (stmt.type)
    {
    case CSTMT_EMPTY:
        break;

    case CSTMT_DECL:
        cgen_c_decl(emit, *stmt.decl_.decl);
        return;

    case CSTMT_EXPR:
        cgen_c_expr(emit, stmt.expr_.expr);
        return;

    case CSTMT_BLOCK:
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        vec_foreach(v, &stmt.block_.stmts)
        {
            cgen_c_stmt(emit, v);
            if (cgen_c_should_stmt_endline(v.type))
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

        cgen_c_expr(emit, stmt.if_.expr);
        emit_fmt(emit, ")\n");

        if (stmt.if_.stmt_true->type != CSTMT_BLOCK)
        {
            emit_ident(emit);
            cgen_c_stmt(emit, *stmt.if_.stmt_true);
            emit_deident(emit);
        }
        else
        {
            cgen_c_stmt(emit, *stmt.if_.stmt_true);
        }

        if (stmt.if_.stmt_false->type != CSTMT_EMPTY)
        {
            emit_fmt(emit, "\nelse\n");
            if (stmt.if_.stmt_true->type != CSTMT_BLOCK)
            {
                emit_ident(emit);
                cgen_c_stmt(emit, *stmt.if_.stmt_false);
                emit_deident(emit);
            }
            else
            {
                cgen_c_stmt(emit, *stmt.if_.stmt_false);
            }
        }
        return;

    case CSTMT_FOR:
        emit_fmt(emit, "for (");
        cgen_c_stmt(emit, *stmt.for_.init_stmt);

        emit_fmt(emit, "; ");
        cgen_c_expr(emit, stmt.for_.cond_expr);

        emit_fmt(emit, "; ");
        cgen_c_expr(emit, stmt.for_.iter_expr);

        emit_fmt(emit, ")\n");
        cgen_c_stmt(emit, *stmt.for_.stmt);
        return;

    case CSTMT_WHILE:
        emit_fmt(emit, "while (");
        cgen_c_expr(emit, stmt.while_.expr);
        emit_fmt(emit, ") \n");

        cgen_c_stmt(emit, *stmt.while_.stmt);
        return;

    case CSTMT_DO:
        emit_fmt(emit, "do \n ");

        cgen_c_stmt(emit, *stmt.do_.stmt);

        emit_fmt(emit, "while (");
        cgen_c_expr(emit, stmt.do_.expr);
        emit_fmt(emit, ") \n");
        return;

    case CSTMT_SWITCH:
        emit_fmt(emit, "switch (");
        cgen_c_expr(emit, stmt.while_.expr);
        emit_fmt(emit, ") \n");
        cgen_c_stmt(emit, *stmt.while_.stmt);
        return;

    case CSTMT_RETURN:
        emit_fmt(emit, "return ");
        cgen_c_expr(emit, stmt.return_.expr);
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
        cgen_c_expr(emit, stmt.case_.expr);
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

static void cgen_c_decl_attr(Emit *emit, CDeclAttr attr)
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

void cgen_c_decl(Emit *emit, CDecl decl)
{
    cgen_c_decl_attr(emit, decl.attr);

    if (decl.type == CDECL_TYPE)
    {
        bool is_typedef = decl.name.len != 0;

        if (is_typedef)
        {
            emit_fmt(emit, "typedef ");
        }

        cgen_c_type_start(emit, decl.type_.type);

        if (is_typedef)
        {
            emit_fmt(emit, " {}", decl.name);
        }

        cgen_c_type_end(emit, decl.type_.type);
    }
    else if (decl.type == CDECL_VAR)
    {
        cgen_c_type_start(emit, decl.var_.type);
        emit_fmt(emit, " {} ", decl.name);
        cgen_c_type_end(emit, decl.var_.type);

        if (decl.var_.expr.type != CEXPR_INVALID &&
            decl.var_.expr.type != CEXPR_EMPTY)
        {
            emit_fmt(emit, "=");
            cgen_c_expr(emit, decl.var_.expr);
        }
    }
    else if (decl.type == CDECL_FUNC)
    {
        CType func_type = decl.func_.type;

        // Declarator
        cgen_c_type_start(emit, func_type);
        emit_fmt(emit, " {}", decl.name);
        cgen_c_type_end(emit, func_type);

        emit_fmt(emit, "\n", decl.name);

        // Body
        cgen_c_stmt(emit, decl.func_.body);
    }
}

/* --- CUnit ---------------------------------------------------------------- */

static void cgen_c_include(Emit *emit, CInclude path)
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

void cgen_c_pragma(Emit *emit, CPragma pragma)
{
    emit_fmt(emit, "#pragma ");
    emit_fmt(emit, pragma.text);
    emit_fmt(emit, "\n");
}

void cgen_c_define(Emit *emit, CDefine define)
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
    cgen_c_expr(emit, define.expression);
    emit_fmt(emit, "\n");
}

void cgen_c_unit(Emit *emit, CUnit unit)
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

            cgen_c_include(emit, entry._include);
            break;

        case CUNIT_PRAGMA:
            cgen_c_pragma(emit, entry._pragma);
            break;

        case CUNIT_DECLARATION:
            emit_fmt(emit, "\n");

            cgen_c_decl(emit, entry._decl);

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

            cgen_c_define(emit, entry._define);
            break;

        default:
            break;
        }

        prev_type = entry.type;
    }
}
