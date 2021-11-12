#pragma once

#include <bid/ast.h>

BidIface bid_iface(Alloc *alloc);

void bid_iface_id(BidIface *iface, uint32_t id);

void bid_iface_name(BidIface *iface, Str name);

void bid_alias(BidIface *iface, Str name, BidType type);

void bid_method(BidIface *iface, Str name, BidType request, BidType response);

BidType bid_none(void);

BidType bid_primitive(Str str, Alloc *alloc);

void bid_primitive_arg(BidType *struct_, Str name);

BidType bid_enum(Alloc *alloc);

void bid_enum_constant(BidType *struct_, Str name);

BidType bid_struct(Alloc *alloc);

void bid_struct_member(BidType *struct_, Str name, BidType type);
