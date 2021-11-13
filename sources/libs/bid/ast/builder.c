#include <bid/ast/builder.h>

BidIface bid_iface(Str name, Alloc *alloc)
{
    BidIface iface = bid_iface_barebone(name, alloc);

    iface.errors = bid_enum(alloc);

    bid_enum_member(&iface.errors, str$("SUCCESS"));
    bid_enum_member(&iface.errors, str$("UNEXPECTED_MESSAGE"));
    bid_enum_member(&iface.errors, str$("BAD_COMMUNICATION"));

    return iface;
}

BidIface bid_iface_barebone(Str name, Alloc *alloc)
{
    BidIface iface;

    iface.errors = bid_nil();
    iface.name = str_dup(name, alloc);

    vec_init(&iface.aliases, alloc);
    vec_init(&iface.methods, alloc);

    return iface;
}

void bid_iface_id(BidIface *iface, uint32_t id)
{
    iface->id = id;
}

void bid_alias(BidIface *iface, Str name, BidType type)
{
    BidAlias alias = {name, nullstr, type};
    vec_push(&iface->aliases, alias);
}

void bid_alias_mangled(BidIface *iface, Str name, Str mangled, BidType type)
{
    BidAlias alias = {name, mangled, type};
    vec_push(&iface->aliases, alias);
}

void bid_method(BidIface *iface, Str name, BidType request, BidType response)
{
    BidMethod method = {name, nullstr, request, response};
    vec_push(&iface->methods, method);
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
    return (BidType){
        .type = BID_TYPE_PRIMITIVE,
        .primitive_ = {
            .name = str,
            .mangled = nullstr,
        },
    };
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

void bid_enum_member(BidType *enum_, Str name)
{
    BidEnumMember member;

    member.name = name;
    member.mangled = nullstr;

    vec_push(&enum_->enum_.members, member);
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
