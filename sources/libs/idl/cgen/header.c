#include <brutal/debug.h>
#include <brutal/io.h>
#include <cc/builder.h>
#include <idl/cgen.h>

void idl_cgen_iface_header(CUnit *unit, IdlModule const module, IdlIface const iface, Alloc *alloc)
{
    CType vtable = ctype_struct(alloc);
    CType msgtype = ctype_enum(alloc);

    int i = 0;
    vec_foreach_v(method, &iface.methods)
    {
        Str name = case_change_str(CASE_PASCAL, method.mangled, alloc);
        CType type = idl_cgen_method_type(method, module, alloc);

        ctype_member(&vtable, method.name, ctype_ident_ptr(name, alloc));
        cunit_decl(unit, cdecl_type(name, type));
        cunit_decl(unit, cdecl_func(method.mangled, type, cstmt_empty()));

        ctype_constant(&msgtype, str_fmt(alloc, "MSG_{case:constant}_REQ", method.mangled), cval_unsigned(i++));
        ctype_constant(&msgtype, str_fmt(alloc, "MSG_{case:constant}_RESP", method.mangled), cval_unsigned(i++));
    }

    cunit_decl(unit, cdecl_type(str_fmt(alloc, "{}Msgs", iface.name), msgtype));
    cunit_decl(unit, cdecl_type(str_fmt(alloc, "{}VTable", iface.name), vtable));
    cunit_decl(unit, cdecl_func(str_fmt(alloc, "{case:lower}_provide", iface.name), idl_cgen_provider_type(iface, alloc), cstmt_empty()));
}

CUnit idl_cgen_header(IdlModule const module, Alloc *alloc)
{
    CUnit unit = cunit(alloc);
    cunit_pragma_once(&unit);

    vec_foreach_v(include, &module.includes)
    {
        cunit_include(&unit, true, include);
    }

    vec_foreach_v(alias, &module.aliases)
    {
        if (alias.type.type != IDL_TYPE_CTYPE)
        {
            cunit_decl(&unit, cdecl_type(alias.mangled, idl_cgen_decl_type(alias.type, alloc)));
        }
    }

    vec_foreach_v(iface, &module.ifaces)
    {
        idl_cgen_iface_header(&unit, module, iface, alloc);
    }

    return unit;
}
