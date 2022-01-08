#include <brutal/hash.h>
#include <idl/ast/builder.h>

IdlIface idl_iface(Str name, Alloc *alloc)
{
    IdlIface iface = idl_iface_barebone(name, alloc);

    iface.errors = idl_enum(alloc);

    idl_enum_constant(&iface.errors, str$("SUCCESS"));
    idl_enum_constant(&iface.errors, str$("UNEXPECTED_MESSAGE"));
    idl_enum_constant(&iface.errors, str$("BAD_COMMUNICATION"));

    return iface;
}

IdlIface idl_iface_barebone(Str name, Alloc *alloc)
{
    IdlIface iface;

    iface.errors = idl_nil();
    iface.name = name;
    iface.id = fnv_32(name.buf, name.len, FNV1_32_INIT);

    vec_init(&iface.aliases, alloc);
    vec_init(&iface.methods, alloc);

    return iface;
}

void idl_alias(IdlIface *iface, Str name, IdlType type)
{
    idl_alias_mangled(iface, name, nullstr, type);
}

void idl_alias_mangled(IdlIface *iface, Str name, Str mangled, IdlType type)
{
    IdlAlias alias = {name, mangled, type};
    vec_push(&iface->aliases, alias);
}

void idl_method(IdlIface *iface, Str name, IdlType request, IdlType response)
{
    idl_method_mangled(iface, name, nullstr, request, response);
}

void idl_method_mangled(IdlIface *iface, Str name, Str mangled, IdlType request, IdlType response)
{
    IdlMethod method = {name, mangled, request, response};
    vec_push(&iface->methods, method);
}

IdlType idl_nil(void)
{
    return (IdlType){
        .type = IDL_TYPE_NIL,
    };
}

IdlType idl_primitive(Str str)
{
    return idl_primitive_mangled(str, nullstr);
}

IdlType idl_primitive_mangled(Str str, Str mangled)
{
    return (IdlType){
        .type = IDL_TYPE_PRIMITIVE,
        .primitive_ = {
            .name = str,
            .mangled = mangled,
        },
    };
}

IdlType idl_enum(Alloc *alloc)
{
    IdlEnum enum_;
    vec_init(&enum_.members, alloc);

    return (IdlType){
        .type = IDL_TYPE_ENUM,
        .enum_ = enum_,
    };
}

void idl_enum_constant(IdlType *enum_, Str name)
{
    idl_enum_constant_mangled(enum_, name, nullstr);
}

void idl_enum_constant_mangled(IdlType *enum_, Str name, Str mangled)
{
    IdlEnumMember member;

    member.name = name;
    member.mangled = mangled;

    vec_push(&enum_->enum_.members, member);
}

IdlType idl_struct(Alloc *alloc)
{
    IdlStruct struct_;

    vec_init(&struct_.members, alloc);

    return (IdlType){
        .type = IDL_TYPE_STRUCT,
        .struct_ = struct_,
    };
}

void idl_struct_member(IdlType *struct_, Str name, IdlType type)
{
    IdlStructMember member;

    member.name = name;
    member.type = type;

    vec_push(&struct_->struct_.members, member);
}

IdlType idl_vec(IdlType subtype, Alloc *alloc)
{
    return (IdlType){
        .type = IDL_TYPE_VEC,
        .vec_ = {
            .subtype = alloc_move(alloc, subtype),
        },
    };
}
