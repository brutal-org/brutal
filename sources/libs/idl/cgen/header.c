#include <brutal-fmt>
#include <cc/builder.h>
#include <idl/cgen.h>

void idl_cgen_iface_header(CUnit *unit, IdlModule const module, IdlIface const iface, Alloc *alloc)
{
    CType vtable = ctype_struct(alloc);
    CType msgtype = ctype_enum(alloc);

    cunit_define(unit, fmt_str$(alloc, "IPC_{case-constant}_PROTO", iface.name), cexpr_constant(cval_unsigned(iface.id)), alloc);

    int i = 0;
    vec_foreach_v(method, &iface.methods)
    {
        Str name = case_change_str(CASE_PASCAL, method.name, alloc);
        CType method_type = idl_cgen_method_type(method, module, alloc);
        CType handler_type = idl_cgen_handler_type(method, module, alloc);

        ctype_member(&vtable, method.name, ctype_ident_ptr(name, alloc));
        cunit_decl(unit, cdecl_type(name, handler_type));
        cunit_decl(unit, cdecl_func(fmt_str$(alloc, "{}_rpc", method.name), method_type, cstmt_empty()));

        ctype_constant(&msgtype, fmt_str$(alloc, "MSG_{case-constant}_REQ", method.name), cval_unsigned(i++));
        ctype_constant(&msgtype, fmt_str$(alloc, "MSG_{case-constant}_RESP", method.name), cval_unsigned(i++));
    }

    cunit_decl(unit, cdecl_type(fmt_str$(alloc, "{}Msgs", iface.name), msgtype));
    cunit_decl(unit, cdecl_type(fmt_str$(alloc, "{}VTable", iface.name), vtable));
    cunit_decl(unit, cdecl_func(fmt_str$(alloc, "{case-snake}_provide", iface.name), idl_cgen_provider_type(iface, alloc), cstmt_empty()));
}

CUnit idl_cgen_header(IdlModule const module, Alloc *alloc)
{
    CUnit unit = cunit(alloc);
    cunit_pragma_once(&unit);

    idl_cgen_includes(&unit, module, alloc);

    vec_foreach_v(alias, &module.aliases)
    {
        if (alias.type.type != IDL_TYPE_CTYPE)
        {
            cunit_decl(&unit, cdecl_type(alias.name, idl_cgen_decl_type(alias.type, alloc)));
        }

        if (!idl_attrs_has(alias.attrs, str$("packfn")))
        {
            cunit_decl(&unit, idl_cgen_pack_decl(alias, alloc));
            cunit_decl(&unit, idl_cgen_unpack_decl(alias, alloc));
        }
    }

    vec_foreach_v(iface, &module.ifaces)
    {
        idl_cgen_iface_header(&unit, module, iface, alloc);
    }

    return unit;
}
