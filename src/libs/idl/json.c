#include <idl/json.h>
#include <brutal-debug>

Json idl_jgen_ctype(IdlCtype ctype_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("IdlCtype"), alloc);
    json_put(&json, str$("name"), json_str(ctype_.name));

    return json;
}

Json idl_jgen_primitive(IdlPrimitive primitive_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("IdlPrimitive"), alloc);

    json_put(&json, str$("name"), json_str(primitive_.name));

    return json;
}

Json idl_jgen_enum(IdlEnum enum_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("IdlEnum"), alloc);
    Json members_json = json_array(alloc);

    vec_foreach_v(member, &enum_.members)
    {
        Json member_json = json_object(alloc);

        json_put(&member_json, str$("name"), json_str(member.name));

        json_append(&members_json, member_json);
    }

    json_put(&json, str$("members"), members_json);

    return json;
}

Json idl_jgen_struct(IdlStruct struct_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("IdlStruct"), alloc);
    Json members_json = json_array(alloc);

    vec_foreach_v(member, &struct_.members)
    {
        Json member_json = json_object(alloc);

        json_put(&member_json, str$("name"), json_str(member.name));
        json_put(&member_json, str$("type"), idl_jgen_type(member.type, alloc));

        json_append(&members_json, member_json);
    }

    json_put(&json, str$("members"), members_json);

    return json;
}

Json idl_jgen_vec(IdlVec const vec_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("IdlVec"), alloc);

    json_put(&json, str$("subtype"), idl_jgen_type(*vec_.subtype, alloc));

    return json;
}

Json idl_jgen_type(IdlType const type, Alloc *alloc)
{
    switch (type.type)
    {
    case IDL_TYPE_NIL:
        return json_object_with_type(str$("IdlNil"), alloc);

    case IDL_TYPE_CTYPE:
        return idl_jgen_ctype(type.ctype_, alloc);

    case IDL_TYPE_PRIMITIVE:
        return idl_jgen_primitive(type.primitive_, alloc);

    case IDL_TYPE_ENUM:
        return idl_jgen_enum(type.enum_, alloc);

    case IDL_TYPE_STRUCT:
        return idl_jgen_struct(type.struct_, alloc);

    case IDL_TYPE_VEC:
        return idl_jgen_vec(type.vec_, alloc);

    default:
        panic$("Unknown type type {}", type.type);
    }
}

Json idl_jgen_alias(IdlAlias const alias, Alloc *alloc)
{
    Json json = json_object_with_type(str$("IdlAlias"), alloc);

    json_put(&json, str$("name"), json_str(alias.name));
    json_put(&json, str$("type"), idl_jgen_type(alias.type, alloc));

    return json;
}

Json idl_jgen_method(IdlMethod const method, Alloc *alloc)
{
    Json json = json_object_with_type(str$("IdlMethod"), alloc);

    json_put(&json, str$("name"), json_str(method.name));
    json_put(&json, str$("request"), idl_jgen_type(method.request, alloc));
    json_put(&json, str$("response"), idl_jgen_type(method.response, alloc));

    return json;
}

Json idl_jgen_iface(IdlIface const iface, Alloc *alloc)
{
    Json methods_json = json_array(alloc);

    vec_foreach_v(method, &iface.methods)
    {
        json_append(&methods_json, idl_jgen_method(method, alloc));
    }

    Json iface_json = json_object_with_type(str$("IdlIface"), alloc);

    json_put(&iface_json, str$("id"), json_number(iface.id));
    json_put(&iface_json, str$("name"), json_str(iface.name));
    json_put(&iface_json, str$("methods"), methods_json);

    return iface_json;
}

Json idl_jgen_module(IdlModule const module, Alloc *alloc)
{
    Json imports_json = json_array(alloc);
    Json includes_json = json_array(alloc);
    Json aliases_json = json_array(alloc);
    Json ifaces_json = json_array(alloc);

    vec_foreach_v(import, &module.imports)
    {
        json_append(&imports_json, json_str(import.name));
    }

    vec_foreach_v(include, &module.includes)
    {
        json_append(&includes_json, json_str(include));
    }

    vec_foreach_v(alias, &module.aliases)
    {
        json_append(&aliases_json, idl_jgen_alias(alias, alloc));
    }

    vec_foreach_v(iface, &module.ifaces)
    {
        json_append(&ifaces_json, idl_jgen_iface(iface, alloc));
    }

    Json module_json = json_object_with_type(str$("IdlModule"), alloc);

    json_put(&module_json, str$("name"), json_str(module.name));
    json_put(&module_json, str$("import"), imports_json);
    json_put(&module_json, str$("includes"), includes_json);
    json_put(&module_json, str$("errors"), idl_jgen_type(module.errors, alloc));
    json_put(&module_json, str$("aliases"), aliases_json);
    json_put(&module_json, str$("ifaces"), ifaces_json);

    return module_json;
}
