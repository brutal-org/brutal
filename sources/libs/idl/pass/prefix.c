#include <brutal/debug.h>
#include <brutal/io.h>
#include <idl/ast.h>
#include <idl/ast/builder.h>
#include <idl/parse.h>

static IdlType prefix_type(IdlType type, IdlModule module, Alloc *alloc)
{
    IdlType prefixed;

    switch (type.type)
    {
    case IDL_TYPE_NIL:
        return idl_nil();

    case IDL_TYPE_CTYPE:
    {
        return idl_ctype(type.ctype_.name);
    }

    case IDL_TYPE_PRIMITIVE:
    {
        Str name = type.primitive_.name;

        IdlAlias alias;
        if (!idl_resolve_alias(module, name, &alias))
        {
            panic$("Unknow type \"{}\"", name);
        }

        return idl_primitive_resolved(name, alias, alloc);
    }

    case IDL_TYPE_ENUM:
    {
        prefixed = idl_enum(alloc);

        vec_foreach_v(membr, &type.enum_.members)
        {
            Str mangled = str_fmt(alloc, "{case:constant}_{case:constant}", module.name, membr.name);
            idl_enum_constant_mangled(&prefixed, membr.name, mangled);
        }

        return prefixed;
    }

    case IDL_TYPE_STRUCT:
    {
        prefixed = idl_struct(alloc);

        vec_foreach_v(membr, &type.struct_.members)
        {
            idl_struct_member(&prefixed, membr.name, prefix_type(membr.type, module, alloc));
        }

        return prefixed;
    }

    case IDL_TYPE_VEC:
    {
        return idl_vec(prefix_type(*type.vec_.subtype, module, alloc), alloc);
    }

    default:
        panic$("Unknow type type {}", type.type);
    }
}

IdlIface prefix_iface(IdlModule *module, IdlIface iface, Alloc *alloc)
{
    IdlIface prefixed = idl_iface(iface.name, iface.attrs, alloc);
    prefixed.id = iface.id;

    vec_foreach_v(method, &iface.methods)
    {
        Str name = str_fmt(alloc, "{case:snake}_{case:snake}_{case:snake}", module->name, iface.name, method.name);

        IdlType prefixed_request = prefix_type(method.request, *module, alloc);
        IdlType prefixed_response = prefix_type(method.response, *module, alloc);

        if (prefixed_request.type != IDL_TYPE_PRIMITIVE && prefixed_request.type != IDL_TYPE_NIL)
        {
            Str request_name = str_fmt(alloc, "{case:pascal}Request", module->name, method.name);
            Str request_mangled = str_fmt(alloc, "{case:pascal}{case:pascal}Request", module->name, method.name);
            IdlAlias request_alias = idl_alias_mangled(request_name, request_mangled, idl_attrs(alloc), prefixed_request);
            idl_module_alias(module, request_alias);
            prefixed_request = idl_primitive_resolved(name, request_alias, alloc);
        }

        if (prefixed_response.type != IDL_TYPE_PRIMITIVE && prefixed_response.type != IDL_TYPE_NIL)
        {
            Str response_name = str_fmt(alloc, "{case:pascal}Response", module->name, method.name);
            Str response_mangled = str_fmt(alloc, "{case:pascal}{case:pascal}Response", module->name, method.name);
            IdlAlias response_alias = idl_alias_mangled(response_name, response_mangled, idl_attrs(alloc), prefixed_response);
            idl_module_alias(module, response_alias);
            prefixed_response = idl_primitive_resolved(name, response_alias, alloc);
        }

        idl_iface_methode(&prefixed, idl_method_mangled(method.name, name, method.attrs, prefixed_request, prefixed_response));
    }

    return prefixed;
}

IdlModule idl_pass_prefix(IdlModule module, Alloc *alloc)
{
    IdlModule prefixed = idl_module(module.name, alloc);

    Str error_name = str_fmt(alloc, "{case:pascal}Error", module.name);
    IdlAlias error_alias = idl_alias_mangled(str$("Error"), error_name, idl_attrs(alloc), prefix_type(module.errors, prefixed, alloc));
    idl_module_alias(&prefixed, error_alias);
    prefixed.errors = idl_primitive_resolved(str$("Error"), error_alias, alloc);

    vec_foreach_v(import, &module.imports)
    {
        IoFile source_file;
        UNWRAP_OR_PANIC(io_file_open(&source_file, str_fmt(alloc, "sources/protos/{}.idl", import.name)), "File not found!");

        Buf source_buf;
        buf_init(&source_buf, 512, alloc);

        io_copy(io_file_reader(&source_file), buf_writer(&source_buf));

        Scan scan;
        scan_init(&scan, buf_str(&source_buf));

        import.module = idl_parse(&scan, alloc);
        vec_push(&prefixed.imports, import);
    }

    vec_foreach_v(alias, &module.aliases)
    {
        Str name = alias.name;

        if (!idl_attrs_has(alias.attrs, str$("no_prefix")))
        {
            name = str_fmt(alloc, "{case:pascal}{case:pascal}", module.name, alias.name);
        }

        idl_module_alias(&prefixed, idl_alias_mangled(alias.name, name, alias.attrs, prefix_type(alias.type, prefixed, alloc)));
    }

    vec_foreach_v(iface, &module.ifaces)
    {
        IdlIface prefixed_iface = prefix_iface(&prefixed, iface, alloc);
        idl_module_iface(&prefixed, prefixed_iface);
    }

    return prefixed;
}
