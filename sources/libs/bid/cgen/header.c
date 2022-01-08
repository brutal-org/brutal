#include <bid/cgen.h>
#include <brutal/io.h>
#include <cc/builder.h>

CUnit bid_cgen_header(MAYBE_UNUSED BidIface const iface, Alloc *alloc)
{
    CUnit unit = cunit(alloc);
    cunit_pragma_once(&unit);

    cunit_include(&unit, true, str$("bal/ipc.h"));

    vec_foreach_v(alias, &iface.aliases)
    {
        cunit_decl(&unit, cdecl_type(alias.mangled, bid_cgen_decl_type(alias.type, alloc)));
    }

    CType vtable = ctype_struct(alloc);
    CType msgtype = ctype_enum(alloc);

    int i = 0;
    vec_foreach_v(method, &iface.methods)
    {
        Str name = case_change_str(CASE_PASCAL, method.mangled, alloc);
        CType type = bid_cgen_method_type(method, iface, alloc);

        ctype_member(&vtable, method.name, ctype_ident_ptr(name, alloc));
        cunit_decl(&unit, cdecl_type(name, type));
        cunit_decl(&unit, cdecl_func(method.mangled, type, cstmt_empty()));

        ctype_constant(&msgtype, str_fmt(alloc, "MSG_{case:constant}_REQ", method.mangled), cval_unsigned(i++));
        ctype_constant(&msgtype, str_fmt(alloc, "MSG_{case:constant}_RESP", method.mangled), cval_unsigned(i++));
    }

    cunit_decl(&unit, cdecl_type(str_fmt(alloc, "{}Msgs", iface.name), msgtype));
    cunit_decl(&unit, cdecl_type(str_fmt(alloc, "{}VTable", iface.name), vtable));
    cunit_decl(&unit, cdecl_func(str_fmt(alloc, "{case:lower}_provide", iface.name), bid_cgen_provider_type(iface, alloc), cstmt_empty()));

    return unit;
}
