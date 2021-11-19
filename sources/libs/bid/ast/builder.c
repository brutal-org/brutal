#include <bid/ast/builder.h>
#include <brutal/hash.h>

BidIface bid_iface(Str name, Alloc *alloc)
{
    BidIface iface = bid_iface_barebone(name, alloc);

    iface.errors = bid_enum(alloc);

    bid_enum_constant(&iface.errors, str$("SUCCESS"));
    bid_enum_constant(&iface.errors, str$("UNEXPECTED_MESSAGE"));
    bid_enum_constant(&iface.errors, str$("BAD_COMMUNICATION"));

    return iface;
}

BidIface bid_iface_barebone(Str name, Alloc *alloc)
{
    BidIface iface;

    iface.errors = bid_nil();
    iface.name = name;
    iface.id = fnv_32(name.buf, name.len, FNV1_32_INIT);

    vec_init(&iface.aliases, alloc);
    vec_init(&iface.methods, alloc);

    return iface;
}

void bid_alias(BidIface *iface, Str name, BidType type)
{
    bid_alias_mangled(iface, name, nullstr, type);
}

void bid_alias_mangled(BidIface *iface, Str name, Str mangled, BidType type)
{
    BidAlias alias = {name, mangled, type};
    vec_push(&iface->aliases, alias);
}

void bid_method(BidIface *iface, Str name, BidType request, BidType response)
{
    bid_method_mangled(iface, name, nullstr, request, response);
}

void bid_method_mangled(BidIface *iface, Str name, Str mangled, BidType request, BidType response)
{
    BidMethod method = {name, mangled, request, response};
    vec_push(&iface->methods, method);
}

BidType bid_nil(void)
{
    return (BidType){
        .type = BID_TYPE_NIL,
    };
}

BidType bid_primitive(Str str)
{
    return bid_primitive_mangled(str, nullstr);
}

BidType bid_primitive_mangled(Str str, Str mangled)
{
    return (BidType){
        .type = BID_TYPE_PRIMITIVE,
        .primitive_ = {
            .name = str,
            .mangled = mangled,
        },
    };
}

BidType bid_enum(Alloc *alloc)
{
    BidEnum enum_;
    vec_init(&enum_.members, alloc);

    return (BidType){
        .type = BID_TYPE_ENUM,
        .enum_ = enum_,
    };
}

void bid_enum_constant(BidType *enum_, Str name)
{
    bid_enum_constant_mangled(enum_, name, nullstr);
}

void bid_enum_constant_mangled(BidType *enum_, Str name, Str mangled)
{
    BidEnumMember member;

    member.name = name;
    member.mangled = mangled;

    vec_push(&enum_->enum_.members, member);
}

BidType bid_struct(Alloc *alloc)
{
    BidStruct struct_;

    vec_init(&struct_.members, alloc);

    return (BidType){
        .type = BID_TYPE_STRUCT,
        .struct_ = struct_,
    };
}

void bid_struct_member(BidType *struct_, Str name, BidType type)
{
    BidStructMember member;

    member.name = name;
    member.type = type;

    vec_push(&struct_->struct_.members, member);
}

BidType bid_vec(BidType subtype, Alloc *alloc)
{
    return (BidType){
        .type = BID_TYPE_VEC,
        .vec_ = {
            .subtype = alloc_move(alloc, subtype),
        },
    };
}
