#include <cc/trans.h>

static void ctran_member(Emit *emit, CTypeMember type)
{
    ctran_type_start(emit, type.type);
    emit_fmt(emit, " {}", type.name);
    ctran_type_end(emit, type.type);
}

static void ctran_constant(Emit *emit, CTypeConstant member)
{
    emit_fmt(emit, "{}", member.name);

    if (member.value.type != CVAL_INVALID)
    {
        emit_fmt(emit, " = ");
        ctran_value(emit, member.value);
    }
}

static void ctran_type_attr(Emit *emit, CTypeAttr attr)
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

void ctran_type(Emit *emit, CType type)
{
    ctran_type_start(emit, type);
    ctran_type_end(emit, type);
}

void ctran_type_start(Emit *emit, CType type)
{
    if (type.type == CTYPE_NAME)
    {
        emit_fmt(emit, type.name);
        ctran_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_PTR)
    {
        ctran_type_start(emit, *type.ptr_.subtype);
        emit_fmt(emit, "*");
        ctran_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_PARENT)
    {
        ctran_type_start(emit, *type.ptr_.subtype);
        emit_fmt(emit, "(");
    }
    else if (type.type == CTYPE_FUNC)
    {
        ctran_type(emit, *type.func_.ret); // return
    }
    else if ((type.type == CTYPE_STRUCT || type.type == CTYPE_UNION) &&
             type.struct_.members.len != 0)
    {
        if (type.type == CTYPE_STRUCT)
        {
            emit_fmt(emit, "struct");
        }
        else
        {
            emit_fmt(emit, "union");
        }

        if (str_any(type.name))
        {
            emit_fmt(emit, " {}", type.name);
        }

        ctran_type_attr(emit, type.attr);
        emit_fmt(emit, "\n{{\n");
        emit_ident(emit);

        vec_foreach_v(v, &type.struct_.members)
        {
            ctran_member(emit, v);
            emit_fmt(emit, ";\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");
    }
    else if (type.type == CTYPE_ENUM)
    {
        emit_fmt(emit, "enum", type.name);

        if (str_any(type.name))
        {
            emit_fmt(emit, " {}", type.name);
        }

        emit_fmt(emit, "\n{{\n");
        emit_ident(emit);

        vec_foreach_v(v, &type.enum_.constants)
        {
            ctran_constant(emit, v);
            emit_fmt(emit, ",\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");

        ctran_type_attr(emit, type.attr);
    }
    else if (type.type == CTYPE_ARRAY)
    {
        ctran_type_attr(emit, type.attr);
        ctran_type_start(emit, *type.array_.subtype);
    }
    else
    {
        emit_fmt(emit, ctype_to_str(type.type));
    }
}

void ctran_func_params(Emit *emit, CType type)
{
    emit_fmt(emit, "(");

    bool first = true;
    vec_foreach_v(v, &type.func_.params)
    {
        if (!first)
        {
            emit_fmt(emit, ", ");
        }
        first = false;
        ctran_member(emit, v);
    }
    emit_fmt(emit, ")");
}

void ctran_type_end(Emit *emit, CType type)
{
    if (type.type == CTYPE_PTR)
    {
        ctran_type_end(emit, *type.ptr_.subtype);
    }
    else if (type.type == CTYPE_PARENT)
    {
        emit_fmt(emit, ")");
        ctran_type_end(emit, *type.ptr_.subtype);
    }
    else if (type.type == CTYPE_FUNC)
    {
        ctran_func_params(emit, type);
        ctran_type_attr(emit, type.attr);
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
        ctran_type_end(emit, *type.ptr_.subtype);
    }
}
