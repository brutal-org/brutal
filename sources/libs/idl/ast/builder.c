#include <idl/ast/builder.h>
#include <ipc/ipc.h>
#include <brutal-hash>

/* --- Types ---------------------------------------------------------------- */

IdlType idl_nil(void)
{
    return (IdlType){
        .type = IDL_TYPE_NIL,
    };
}

IdlType idl_ctype(Str ctype)
{
    return (IdlType){
        .type = IDL_TYPE_CTYPE,
        .ctype_ = {
            .name = ctype,
        },
    };
}

IdlType idl_primitive(Str str)
{
    return (IdlType){
        .type = IDL_TYPE_PRIMITIVE,
        .primitive_ = {
            .name = str,
        },
    };
}

IdlType idl_primitive_resolved(Str str, IdlAlias alias, Alloc *alloc)
{
    return (IdlType){
        .type = IDL_TYPE_PRIMITIVE,
        .primitive_ = {
            .name = str,
            .alias = alloc_move(alloc, alias),
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

void idl_enum_constant(IdlType *enum_, Str name, int value)
{
    IdlEnumMember member = {
        .name = name,
        .value = value,
    };

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
/* --- Attributes ----------------------------------------------------------- */

IdlAttrs idl_attrs(Alloc *alloc)
{
    IdlAttrs attrs = {};
    vec_init(&attrs, alloc);
    return attrs;
}

void idl_attrs_append(IdlAttrs *attrs, IdlAttr attr)
{
    vec_push(attrs, attr);
}

IdlAttr idl_attr(Str name, Alloc *alloc)
{
    IdlAttr attr = {};
    attr.name = name;
    vec_init(&attr.args, alloc);
    return attr;
}

void idl_attr_append(IdlAttr *attr, Str data)
{
    vec_push(&attr->args, data);
}

/* --- Methods -------------------------------------------------------------- */

IdlMethod idl_method(Str name, IdlAttrs attrs, IdlType request, IdlType response)
{
    IdlMethod method = {
        .name = name,
        .request = request,
        .response = response,
        .attrs = attrs,
    };

    return method;
}

/* --- Aliases -------------------------------------------------------------- */

IdlAlias idl_alias(Str name, IdlAttrs attrs, IdlType type)
{
    IdlAlias alias = {
        .name = name,
        .type = type,
        .attrs = attrs,
    };

    return alias;
}

/* --- Interfaces ----------------------------------------------------------- */

IdlIface idl_iface(Str name, IdlAttrs attrs, Alloc *alloc)
{
    IdlIface iface = {};

    iface.name = name;
    iface.attrs = attrs;

    vec_init(&iface.methods, alloc);

    return iface;
}

void idl_iface_methode(IdlIface *iface, IdlMethod method)
{
    vec_push(&iface->methods, method);
}

/* --- Modules -------------------------------------------------------------- */

IdlModule idl_module(Str name, Alloc *alloc)
{
    IdlModule module = {
        .name = name,
    };

    module.errors = idl_enum(alloc);

    idl_enum_constant(&module.errors, str$("SUCCESS"), IPC_SUCCESS);
    idl_enum_constant(&module.errors, str$("UNEXPECTED_MESSAGE"), IPC_UNEXPECTED_MESSAGE);
    idl_enum_constant(&module.errors, str$("BAD_COMMUNICATION"), IPC_BAD_COMMUNICATION);

    vec_init(&module.aliases, alloc);
    vec_init(&module.imports, alloc);
    vec_init(&module.includes, alloc);
    vec_init(&module.aliases, alloc);
    vec_init(&module.ifaces, alloc);

    return module;
}

void idl_module_import(IdlModule *module, Str name)
{
    IdlImport import = {
        .name = name,
    };

    vec_push(&module->imports, import);
}

void idl_module_include(IdlModule *module, Str name)
{
    vec_push(&module->includes, name);
}

void idl_module_alias(IdlModule *module, IdlAlias alias)
{
    vec_push(&module->aliases, alias);
}

void idl_module_iface(IdlModule *module, IdlIface iface)
{
    vec_push(&module->ifaces, iface);
}
