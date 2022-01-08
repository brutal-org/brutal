#include <brutal/debug.h>
#include <brutal/io.h>
#include <idl/ast.h>
#include <idl/ast/builder.h>

static IdlType prefix_type(IdlType type, IdlIface iface, Alloc *alloc)
{
    IdlType prefixed;

    switch (type.type)
    {

    case IDL_TYPE_NIL:
        return idl_nil();

    case IDL_TYPE_PRIMITIVE:
    {
        Str name = type.primitive_.name;
        Str mangled = name;

        if (idl_type_in_interface(&iface, type.primitive_.name))
        {
            mangled = str_fmt(alloc, "{case:pascal}{case:pascal}", iface.name, type.primitive_.name);
        }

        return idl_primitive_mangled(name, mangled);
    }

    case IDL_TYPE_ENUM:
    {
        prefixed = idl_enum(alloc);

        vec_foreach_v(membr, &type.enum_.members)
        {
            Str mangled = str_fmt(alloc, "{case:constant}_{case:constant}", iface.name, membr.name);
            idl_enum_constant_mangled(&prefixed, membr.name, mangled);
        }

        return prefixed;
    }

    case IDL_TYPE_STRUCT:
    {
        prefixed = idl_struct(alloc);

        vec_foreach_v(membr, &type.struct_.members)
        {
            idl_struct_member(&prefixed, membr.name, prefix_type(membr.type, iface, alloc));
        }

        return prefixed;
    }

    case IDL_TYPE_VEC:
    {
        return idl_vec(prefix_type(*type.vec_.subtype, iface, alloc), alloc);
    }

    default:
        panic$("Unknow type type {}", type.type);
    }
}

IdlIface idl_pass_prefix(IdlIface iface, Alloc *alloc)
{
    IdlIface prefixed = idl_iface(iface.name, alloc);

    prefixed.id = iface.id;

    Str error_name = str_fmt(alloc, "{case:pascal}Error", iface.name);
    idl_alias_mangled(&prefixed, str$("Error"), error_name, prefix_type(iface.errors, iface, alloc));
    prefixed.errors = idl_primitive_mangled(str$("Error"), error_name);

    vec_foreach_v(alias, &iface.aliases)
    {
        Str name = str_fmt(alloc, "{case:pascal}{case:pascal}", iface.name, alias.name);
        idl_alias_mangled(&prefixed, alias.name, name, prefix_type(alias.type, iface, alloc));
    }

    vec_foreach_v(methods, &iface.methods)
    {
        Str name = str_fmt(alloc, "{case:snake}_{case:snake}", iface.name, methods.name);

        IdlType prefixed_request = prefix_type(methods.request, iface, alloc);
        IdlType prefixed_response = prefix_type(methods.response, iface, alloc);

        if (prefixed_request.type != IDL_TYPE_PRIMITIVE && prefixed_request.type != IDL_TYPE_NIL)
        {
            Str request_name = str_fmt(alloc, "{case:pascal}Request", iface.name, methods.name);
            Str mangled = str_fmt(alloc, "{case:pascal}{case:pascal}Request", iface.name, methods.name);
            idl_alias_mangled(&prefixed, request_name, mangled, prefixed_request);
            prefixed_request = idl_primitive_mangled(name, mangled);
        }

        if (prefixed_response.type != IDL_TYPE_PRIMITIVE && prefixed_response.type != IDL_TYPE_NIL)
        {
            Str response_name = str_fmt(alloc, "{case:pascal}Response", iface.name, methods.name);
            Str mangled = str_fmt(alloc, "{case:pascal}{case:pascal}Response", iface.name, methods.name);
            idl_alias_mangled(&prefixed, response_name, mangled, prefixed_response);
            prefixed_response = idl_primitive_mangled(name, mangled);
        }

        idl_method_mangled(&prefixed, methods.name, name, prefixed_request, prefixed_response);
    }

    return prefixed;
}
