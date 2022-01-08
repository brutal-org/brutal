#include <bid/builtin.h>
#include <bid/cgen.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <cc/builder.h>

Str bid_cgen_unpack_name(Str name, Alloc *alloc)
{
    BidBuiltinType *builtin = bid_lookup_builtin(name);

    if (builtin)
    {
        return builtin->unpack_fn;
    }

    return str_fmt(alloc, "{case:snake}_unpack", name);
}

CExpr bid_cgen_unpack_ref(Str name, Alloc *alloc)
{
    return cexpr_cast(cexpr_ident(bid_cgen_unpack_name(name, alloc)), ctype_ident_ptr(str$("BalUnpackFn"), alloc), alloc);
}

CType bid_cgen_unpack_type(BidAlias alias, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);
    ctype_member(&ctype, str$("self"), ctype_ident_ptr(str$("BalUnpack"), alloc));
    ctype_member(&ctype, str$("data"), ctype_ident_ptr(alias.mangled, alloc));
    return ctype;
}

void bid_cgen_unpack_body(CStmt *block, BidType type, CExpr path, Alloc *alloc)
{
    switch (type.type)
    {
    case BID_TYPE_NIL:
        break;

    case BID_TYPE_PRIMITIVE:
    {
        CExpr expr = cexpr_call(alloc, cexpr_ident(bid_cgen_unpack_name(type.primitive_.mangled, alloc)));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);

        cstmt_block_add(block, cstmt_expr(expr));
        break;
    }

    case BID_TYPE_ENUM:
    {
        CExpr expr = cexpr_call(alloc, cexpr_ident(bid_cgen_unpack_name(str$("Enum"), alloc)));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, cexpr_cast(path, ctype_ident_ptr(str$("int"), alloc), alloc));

        cstmt_block_add(block, cstmt_expr(expr));
        break;
    }

    case BID_TYPE_STRUCT:
    {
        vec_foreach_v(member, &type.struct_.members)
        {
            bid_cgen_unpack_body(block, member.type, cexpr_ref(cexpr_ptr_access(path, cexpr_ident(member.name), alloc), alloc), alloc);
        }
        break;
    }

    case BID_TYPE_VEC:
    {
        BidType subtype = *type.vec_.subtype;
        assert_truth(subtype.type == BID_TYPE_PRIMITIVE);

        CExpr expr = cexpr_call(alloc, cexpr_ident(str$("bal_unpack_slice")));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);
        cexpr_member(&expr, cexpr_ident(bid_cgen_unpack_name(subtype.primitive_.mangled, alloc)));
        cstmt_block_add(block, cstmt_expr(expr));
        break;
    }

    default:
        panic$("Unknow type type {}", type.type);
    }
}

CDecl bid_cgen_unpack_func(BidAlias alias, Alloc *alloc)
{
    Str name = bid_cgen_unpack_name(alias.mangled, alloc);
    CType type = bid_cgen_unpack_type(alias, alloc);
    CStmt body = cstmt_block(alloc);
    bid_cgen_unpack_body(&body, alias.type, cexpr_ident(str$("data")), alloc);

    return cdecl_func(name, type, body);
}
