#include <brutal-fmt>
#include <cc/builder.h>
#include <idl/cgen.h>

void idl_cgen_iface_source(CUnit *unit, IdlModule const module, IdlIface const iface, Alloc *alloc)
{
    vec_foreach_v(method, &iface.methods)
    {
        CType type = idl_cgen_method_type(method, module, alloc);
        CStmt body = idl_cgen_method_body(method, iface, alloc);

        cunit_decl(unit, cdecl_func(fmt_str$(alloc, "{}_rpc", method.name), type, body));
    }

    cunit_decl(unit, idl_cgen_dispatch_func(iface, alloc));
    cunit_decl(unit, idl_cgen_provider_func(iface, alloc));
}

CUnit idl_cgen_source(IdlModule const module, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    idl_cgen_includes(&unit, module, alloc);
    cunit_include(&unit, false, fmt_str$(alloc, "{case-snake}.h", module.name));

    vec_foreach_v(alias, &module.aliases)
    {
        if (!idl_attrs_has(alias.attrs, str$("packfn")))
        {
            cunit_decl(&unit, idl_cgen_pack_func(alias, alloc));
            cunit_decl(&unit, idl_cgen_unpack_func(alias, alloc));
        }
    }

    vec_foreach_v(iface, &module.ifaces)
    {
        idl_cgen_iface_source(&unit, module, iface, alloc);
    }

    return unit;
}
