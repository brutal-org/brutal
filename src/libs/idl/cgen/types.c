#include <brutal-fmt>
#include <cc/builder.h>
#include <idl/cgen.h>

CType idl_cgen_decl_primitive(IdlType type)
{
    assert_truth(type.type == IDL_TYPE_PRIMITIVE);
    if (type.primitive_.alias->type.type == IDL_TYPE_CTYPE)
    {
        return ctype_ident(type.primitive_.alias->type.ctype_.name);
    }
    return ctype_ident(type.primitive_.alias->name);
}

CType idl_cgen_decl_enum(IdlType type, Alloc *alloc)
{
    CType ctype = ctype_enum(alloc);

    vec_foreach_v(member, &type.enum_.members)
    {
        ctype_constant(&ctype, member.name, cval_signed(member.value));
    }

    return ctype;
}

CType idl_cgen_decl_struct(IdlType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);

    vec_foreach_v(member, &type.struct_.members)
    {
        ctype_member(&ctype, member.name, idl_cgen_decl_type(member.type, alloc));
    }

    return ctype;
}

CType idl_cgen_decl_vec(IdlType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);
    CType buf_type = ctype_ptr(idl_cgen_decl_type(*type.vec_.subtype, alloc), alloc);
    CType size_type = ctype_ident(str$("size_t"));

    ctype_member(&ctype, str$("buf"), buf_type);
    ctype_member(&ctype, str$("len"), size_type);

    return ctype;
}

CType idl_cgen_decl_type(IdlType type, Alloc *alloc)
{
    switch (type.type)
    {
    case IDL_TYPE_NIL:
        return ctype_void();

    case IDL_TYPE_CTYPE:
        return ctype_ident(type.ctype_.name);

    case IDL_TYPE_PRIMITIVE:
        return idl_cgen_decl_primitive(type);

    case IDL_TYPE_ENUM:
        return idl_cgen_decl_enum(type, alloc);

    case IDL_TYPE_STRUCT:
        return idl_cgen_decl_struct(type, alloc);

    case IDL_TYPE_VEC:
        return idl_cgen_decl_vec(type, alloc);

    default:
        panic$("Unknow type type {}", type.type);
    }
}

CType idl_cgen_alias_type(IdlAlias alias)
{
    IdlType type = alias.type;

    if (type.type == IDL_TYPE_CTYPE)
    {
        return ctype_ident(type.ctype_.name);
    }
    else
    {
        return ctype_ident(alias.name);
    }
}
