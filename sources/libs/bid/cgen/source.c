#include <bid/cgen.h>
#include <brutal/io.h>
#include <cc/builder.h>

CUnit bid_cgen_source(MAYBE_UNUSED BidIface const iface, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    cunit_include(&unit, false, str_fmt(alloc, "{case:snake}.h", iface.name));

    vec_foreach_v(alias, &iface.aliases)
    {
        cunit_decl(&unit, bid_cgen_pack_func(alias, alloc));
        cunit_decl(&unit, bid_cgen_unpack_func(alias, alloc));
    }

    vec_foreach_v(method, &iface.methods)
    {
        CType type = bid_cgen_method_type(method, iface, alloc);
        CStmt body = bid_cgen_method_body(method, iface, alloc);

        cunit_decl(&unit, cdecl_func(method.mangled, type, body));
    }

    cunit_decl(&unit, bid_cgen_dispatch_func(iface, alloc));
    cunit_decl(&unit, bid_cgen_provider_func(iface, alloc));

    return unit;
}
