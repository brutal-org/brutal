#include <cc/builder.h>
#include <cc/trans.h>

static void ctrans_member(Emit *emit, CTypeMember type)
{
    ctrans_type_start(emit, type.type);
    emit_fmt(emit, " {}", type.name);
    ctrans_type_end(emit, type.type);
}

static void ctrans_constant(Emit *emit, CTypeConst member)
{
    emit_fmt$(emit, "{}", member.name);

    if (member.value.type != CVAL_INVALID)
    {
        emit_fmt(emit, " = ");
        ctrans_value(emit, member.value);
    }
}

static void ctrans_type_attr(Emit *emit, CTypeAttr attr)
{
    if (attr & CTYPE_CONST)
    {
        emit_fmt$(emit, " const");
    }

    if (attr & CTYPE_RESTRICT)
    {
        emit_fmt$(emit, " restrict");
    }

    if (attr & CTYPE_VOLATILE)
    {
        emit_fmt$(emit, " volatile");
    }
}

void ctrans_type(Emit *emit, CType type)
{
    ctrans_type_start(emit, type);
    ctrans_type_end(emit, type);
}

void ctrans_type_start(Emit *emit, CType type)
{
    if (type.type == CTYPE_NAME)
    {
        emit_fmt(emit, type.name);
        ctrans_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_PTR)
    {
        ctrans_type_start(emit, *type.ptr_.subtype);
        emit_fmt(emit, "*");
        ctrans_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_PARENT)
    {
        ctrans_type_start(emit, *type.ptr_.subtype);
        emit_fmt(emit, "(");
    }
    else if (type.type == CTYPE_FUNC)
    {
        ctrans_type(emit, *type.func_.ret); // return
    }
    else if ((type.type == CTYPE_STRUCT || type.type == CTYPE_UNION) &&
             type.struct_.members.len != 0)
    {
        if (type.type == CTYPE_STRUCT)
        {
            emit_fmt$(emit, "struct");
        }
        else
        {
            emit_fmt$(emit, "union");
        }

        if (str_any(type.name))
        {
            emit_fmt$(emit, " {}", type.name);
        }

        ctrans_type_attr(emit, type.attr);
        emit_fmt(emit, "\n{{\n");
        emit_ident(emit);

        vec_foreach_v(v, &type.struct_.members)
        {
            ctrans_member(emit, v);
            emit_fmt(emit, ";\n");
        }

        emit_deident(emit);
        emit_fmt$(emit, "}}");
    }
    else if (type.type == CTYPE_ENUM)
    {
        emit_fmt$(emit, "enum", type.name);

        if (str_any(type.name))
        {
            emit_fmt$(emit, " {}", type.name);
        }

        emit_fmt$(emit, "\n{{\n");
        emit_ident(emit);

        vec_foreach_v(v, &type.enum_.constants)
        {
            ctrans_constant(emit, v);
            emit_fmt(emit, ",\n");
        }

        emit_deident(emit);
        emit_fmt$(emit, "}}");

        ctrans_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_ARRAY)
    {
        ctrans_type_attr(emit, type.attr);
        ctrans_type_start(emit, *type.array_.subtype);
    }
    else
    {
        emit_fmt(emit, ctype_to_str(type));
    }
}

void ctrans_func_params(Emit *emit, CType type)
{
    emit_fmt$(emit, "(");

    bool first = true;
    vec_foreach_v(v, &type.func_.params)
    {
        if (!first)
        {
            emit_fmt$(emit, ", ");
        }
        first = false;
        ctrans_member(emit, v);
    }
    emit_fmt$(emit, ")");
}

void ctrans_type_end(Emit *emit, CType type)
{
    if (type.type == CTYPE_PTR)
    {
        ctrans_type_end(emit, *type.ptr_.subtype);
    }
    else if (type.type == CTYPE_PARENT)
    {
        emit_fmt(emit, ")");
        ctrans_type_end(emit, *type.ptr_.subtype);
    }
    else if (type.type == CTYPE_FUNC)
    {
        ctrans_func_params(emit, type);
        ctrans_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_ARRAY)
    {
        if (type.array_.size == CTYPE_ARRAY_UNBOUNDED)
        {
            emit_fmt$(emit, "[]", type.array_.size);
        }
        else
        {
            emit_fmt$(emit, "[{}]", type.array_.size);
        }
        ctrans_type_end(emit, *type.ptr_.subtype);
    }
}

Str ctrans_type_str(CExpr expr, Alloc *alloc)
{
    Emit emit;
    Buf buf = {};
    buf_init(&buf, 18, alloc);
    emit_init(&emit, buf_writer(&buf));

    ctrans_type(&emit, expr.sema_type);

    emit_deinit(&emit);
    return buf_str(&buf);
}
