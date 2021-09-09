#include <brutal/ds/vec.h>
#include <cc/gen.h>

void c_emit_type(Emit *target, CType type);

const char *ctype_type_to_str[] = {
    [CTYPE_INVALID] = "invalid",
    [CTYPE_VOID] = "void",
    [CTYPE_BOOL] = "bool",
    [CTYPE_PTR] = "",
    [CTYPE_ARRAY] = "",
    [CTYPE_SIGNED] = "signed ",
    [CTYPE_UNSIGNED] = "unsigned ",
    [CTYPE_FLOAT] = "float",
    [CTYPE_STRUCT] = "struct",
    [CTYPE_UNION] = "union",
    [CTYPE_ENUM] = "enum",
    [CTYPE_FUNC] = ""};

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
    emit_fmt(target, "{} ", ctype_type_to_str[type.type]);

    if (type.type == CTYPE_PTR)
    {
        c_emit_type(target, *type.ptr_.subtype);
        emit_fmt(target, "* ");
        return;
    }
    else if (type.type == CTYPE_FUNC)
    {
        c_emit_func_type(target, type, type.name);
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
    // need to implement array ...
    emit_fmt(target, "{} ", type.name);
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

    emit_fmt(target, ";\n");
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
