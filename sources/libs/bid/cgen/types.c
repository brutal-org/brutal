#include <bid/builtin.h>
#include <bid/cgen.h>
#include <brutal/debug.h>
#include <cc/builder.h>

CType bid_cgen_decl_primitive(BidType type)
{
    assert_truth(type.type == BID_TYPE_PRIMITIVE);

    BidBuiltinType *builtin = bid_lookup_builtin(type.primitive_.name);

    if (builtin != nullptr)
    {
        return ctype_ident(builtin->cname);
    }

    return ctype_ident(type.primitive_.mangled);
}

CType bid_cgen_decl_enum(BidType type, Alloc *alloc)
{
    CType ctype = ctype_enum(alloc);

    int i = 0;
    vec_foreach_v(member, &type.enum_.members)
    {
        ctype_constant(&ctype, member.mangled, cval_signed(i++));
    }

    return ctype;
}

CType bid_cgen_decl_struct(BidType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);

    vec_foreach_v(member, &type.struct_.members)
    {
        ctype_member(&ctype, member.name, bid_cgen_decl_type(member.type, alloc));
    }

    return ctype;
}

CType bid_cgen_decl_vec(BidType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);
    CType buf_type = ctype_ptr(bid_cgen_decl_type(*type.vec_.subtype, alloc), alloc);
    CType size_type = ctype_ident(str$("size_t"));

    ctype_member(&ctype, str$("buf"), buf_type);
    ctype_member(&ctype, str$("len"), size_type);

    return ctype;
}

CType bid_cgen_decl_type(BidType type, Alloc *alloc)
{
    switch (type.type)
    {
    case BID_TYPE_NIL:
        return ctype_void();

    case BID_TYPE_PRIMITIVE:
        return bid_cgen_decl_primitive(type);

    case BID_TYPE_ENUM:
        return bid_cgen_decl_enum(type, alloc);

    case BID_TYPE_STRUCT:
        return bid_cgen_decl_struct(type, alloc);

    case BID_TYPE_VEC:
        return bid_cgen_decl_vec(type, alloc);

    default:
        panic$("Unknow type type {}", type.type);
    }
}
