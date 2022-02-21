#include <brutal/debug.h>
#include <brutal/hash.h>
#include <brutal/io.h>
#include <idl/ast.h>
#include <idl/ast/builder.h>
#include <idl/parse.h>

static IdlType idl_sema_type(IdlType type, IdlModule module, Alloc *alloc)
{
    IdlType prefixed;

    switch (type.type)
    {
    case IDL_TYPE_NIL:
    case IDL_TYPE_CTYPE:
        return type;

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
        prefixed = idl_enum(alloc);
        vec_foreach_v(member, &type.enum_.members)
        {
            idl_enum_constant(&prefixed, str_fmt(alloc, "{case:constant}_{case:constant}", module.name, member.name), member.value);
        }
        return prefixed;

    case IDL_TYPE_STRUCT:
        prefixed = idl_struct(alloc);

        vec_foreach_v(membr, &type.struct_.members)
        {
            idl_struct_member(&prefixed, membr.name, idl_sema_type(membr.type, module, alloc));
        }

        return prefixed;

    case IDL_TYPE_VEC:
        return idl_vec(idl_sema_type(*type.vec_.subtype, module, alloc), alloc);

    default:
        panic$("Unknow type type {}", type.type);
    }
}

IdlIface idl_sema_iface(IdlModule *module, IdlIface iface, Alloc *alloc)
{
    IdlIface prefixed = idl_iface(str_fmt(alloc, "{case:pascal}{case:pascal}", module->name, iface.name), iface.attrs, alloc);
    prefixed.id = fnv_32(module->name.buf, module->name.len, FNV1_32_INIT);
    prefixed.id = fnv_32(iface.name.buf, iface.name.len, prefixed.id);

    vec_foreach_v(method, &iface.methods)
    {
        Str name = str_fmt(alloc, "{case:snake}_{case:snake}", prefixed.name, method.name);

        IdlType prefixed_request = idl_sema_type(method.request, *module, alloc);
        IdlType prefixed_response = idl_sema_type(method.response, *module, alloc);

        if (prefixed_request.type != IDL_TYPE_PRIMITIVE && prefixed_request.type != IDL_TYPE_NIL)
        {
            Str request_name = str_fmt(alloc, "{case:pascal}Request", name);
            IdlAlias request_alias = idl_alias(request_name, idl_attrs(alloc), prefixed_request);
            idl_module_alias(module, request_alias);
            prefixed_request = idl_primitive_resolved(request_name, request_alias, alloc);
        }

        if (prefixed_response.type != IDL_TYPE_PRIMITIVE && prefixed_response.type != IDL_TYPE_NIL)
        {
            Str response_name = str_fmt(alloc, "{case:pascal}Response", name);
            IdlAlias response_alias = idl_alias(response_name, idl_attrs(alloc), prefixed_response);
            idl_module_alias(module, response_alias);
            prefixed_response = idl_primitive_resolved(response_name, response_alias, alloc);
        }

        idl_iface_methode(&prefixed, idl_method(name, method.attrs, prefixed_request, prefixed_response));
    }

    return prefixed;
}

IdlModule idl_sema_module(IdlModule module, Alloc *alloc)
{
    IdlModule prefixed = idl_module(module.name, alloc);
    prefixed.includes = module.includes;

    // Don't bother with prefixing errors if there are no interfaces
    if (vec_len(&module.ifaces) > 0)
    {
        Str error_name = str_fmt(alloc, "{case:pascal}Error", module.name);
        IdlAlias error_alias = idl_alias(error_name, idl_attrs(alloc), idl_sema_type(module.errors, prefixed, alloc));
        idl_module_alias(&prefixed, error_alias);
        prefixed.errors = idl_primitive_resolved(str$("Error"), error_alias, alloc);
    }

    vec_foreach_v(import, &module.imports)
    {
        log$("Importing {}...", import.name);
        IoFile source_file;
        UNWRAP_OR_PANIC(io_file_open(&source_file, str_fmt(alloc, "sources/protos/{}.idl", import.name), FILE_OPEN_READ_ONLY), "File not found!");

        Buf source_buf;
        buf_init(&source_buf, 512, alloc);

        io_copy(io_file_reader(&source_file), buf_writer(&source_buf));

        Scan scan;
        scan_init(&scan, buf_str(&source_buf));

        import.module = idl_parse_module(&scan, alloc);
        vec_push(&prefixed.imports, import);
    }

    vec_foreach_v(alias, &module.aliases)
    {
        idl_module_alias(&prefixed, idl_alias(alias.name, alias.attrs, idl_sema_type(alias.type, prefixed, alloc)));
    }

    vec_foreach_v(iface, &module.ifaces)
    {
        // Don't bother with an iface if there are no methods
        if (vec_len(&iface.methods) > 0)
        {
            IdlIface prefixed_iface = idl_sema_iface(&prefixed, iface, alloc);
            idl_module_iface(&prefixed, prefixed_iface);
        }
    }

    return prefixed;
}
