#include <bid/gen/c.h>
#include <brutal/debug.h>
#include <cc/builder.h>

static CType gen_type(BidType type, Alloc *alloc);

/* --- C Header ----------------------------------------- */

static CType gen_decl_enum(BidType type, Alloc *alloc)
{
    CType ctype = ctype_enum(alloc);

    int i = 0;
    vec_foreach(member, &type.enum_.members)
    {
        ctype_constant(&ctype, member.mangled, cval_signed(i++), alloc);
    }

    return ctype;
}

static CType gen_decl_struct(BidType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);

    vec_foreach(member, &type.struct_.members)
    {
        ctype_member(&ctype, member.name, gen_type(member.type, alloc), alloc);
    }

    return ctype;
}

static CType gen_decl_vec(BidType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);

    ctype_member(&ctype, str$("buf"), ctype_ptr(gen_type(*type.vec_.subtype, alloc), alloc), alloc);
    ctype_member(&ctype, str$("len"), ctype_name(str$("len"), alloc), alloc);

    return ctype;
}

static CType gen_decl_type(BidType type, Alloc *alloc)
{
    switch (type.type)
    {
    case BID_TYPE_NIL:
        return ctype_void();

    case BID_TYPE_PRIMITIVE:
        return ctype_name(type.primitive_.mangled, alloc);

    case BID_TYPE_ENUM:
        return gen_decl_enum(type, alloc);

    case BID_TYPE_STRUCT:
        return gen_decl_struct(type, alloc);

    case BID_TYPE_VEC:
        return gen_decl_vec(type, alloc);

    default:
        panic$("Unknow type type {}", type.type);
    }
}

static CType gen_type(BidType type, Alloc *alloc)
{
    assert_truth(type.type == BID_TYPE_PRIMITIVE);
    return ctype_name(type.primitive_.mangled, alloc);
}

static CType gen_func(BidMethod method, BidIface const iface, Alloc *alloc)
{
    CType ctype = ctype_func(gen_type(iface.errors, alloc), alloc);

    ctype_member(&ctype, str$("self"), ctype_ptr(ctype_name(str$("Srv"), alloc), alloc), alloc);

    ctype_member(&ctype, str$("task"), ctype_name(str$("BrTask"), alloc), alloc);

    if (method.request.type != BID_TYPE_NIL)
    {
        ctype_member(&ctype, str$("req"), ctype_ptr(gen_type(method.request, alloc), alloc), alloc);
    }

    if (method.response.type != BID_TYPE_NIL)
    {
        ctype_member(&ctype, str$("resp"), ctype_ptr(gen_type(method.response, alloc), alloc), alloc);
    }

    ctype_member(&ctype, str$("alloc"), ctype_ptr(ctype_name(str$("Alloc"), alloc), alloc), alloc);

    return ctype;
}

CUnit bidgen_c_header(MAYBE_UNUSED BidIface const iface, Alloc *alloc)
{
    CUnit unit = cunit(alloc);
    cunit_member(&unit, cunit_pragma(str$("once"), alloc));

    vec_foreach(alias, &iface.aliases)
    {
        cunit_member(&unit, cunit_decl(cdecl_type(alias.mangled, gen_decl_type(alias.type, alloc), alloc)));
    }

    CType vtable = ctype_struct(alloc);

    vec_foreach(method, &iface.methods)
    {
        Str name = case_change_str(CASE_PASCAL, method.mangled, alloc);
        CType type = gen_func(method, iface, alloc);

        ctype_member(&vtable, method.name, ctype_ptr(ctype_name(name, alloc), alloc), alloc);
        cunit_member(&unit, cunit_decl(cdecl_type(name, type, alloc)));
        cunit_member(&unit, cunit_decl(cdecl_func(method.mangled, type, cstmt_empty(), alloc)));
    }

    cunit_member(&unit, cunit_decl(cdecl_type(str_fmt(alloc, "{}VTable", iface.name), vtable, alloc)));

    return unit;
}

/* --- C Source ----------------------------------------- */

CUnit bidgen_c_source(MAYBE_UNUSED BidIface const iface, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    return unit;
}
