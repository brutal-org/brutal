#include <bid/ast/builder.h>

BidIface bid_iface(Alloc *alloc)
{
    BidIface iface;

    iface.errors = bid_enum(alloc);

    vec_init(&iface.aliases, alloc);
    vec_init(&iface.events, alloc);
    vec_init(&iface.methods, alloc);

    bid_enum_constant(&iface.errors, str$("UNEXPECTED_MESSAGE"));
    bid_enum_constant(&iface.errors, str$("BAD_COMMUNICATION"));
    bid_enum_constant(&iface.errors, str$("SUCCESS"));

    return iface;
}

void bid_iface_id(BidIface *iface, uint32_t id)
{
    iface->id = id;
}

void bid_iface_name(BidIface *iface, Str name)
{
    iface->name = name;
}

void bid_alias(BidIface *iface, Str name, BidType type)
{
    BidAlias alias = {name, type};
    vec_push(&iface->aliases, alias);
}

void bid_event(BidIface *iface, Str name, BidType data)
{
    BidEvent event = {name, data};
    vec_push(&iface->events, event);
}

void bid_method(BidIface *iface, Str name, BidType request, BidType response)
{
    BidMethod method = {name, request, response};
    vec_push(&iface->methods, method);
}

BidType bid_none(void)
{
    return (BidType){
        .type = BID_TYPE_NONE,
    };
}

BidType bid_primitive(Str str, Alloc *alloc)
{
    BidPrimitive primitive_;
    primitive_.name = str;
    vec_init(&primitive_.args, alloc);

    return (BidType){
        .type = BID_TYPE_PRIMITIVE,
        .primitive_ = primitive_,
    };
}

void bid_primitive_arg(BidType *type, Str name)
{
    vec_push(&type->enum_.members, name);
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
    vec_push(&enum_->enum_.members, name);
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
    BidMember member;
    member.name = name;
    member.type = type;

    vec_push(&struct_->struct_.members, member);
}
