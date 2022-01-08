#pragma once

#include <bid/ast.h>
#include <json/objects.h>

Json bid_jgen_primitive(BidPrimitive primitive_, Alloc *alloc);

Json bid_jgen_enum(BidEnum enum_, Alloc *alloc);

Json bid_jgen_struct(BidStruct struct_, Alloc *alloc);

Json bid_jgen_type(BidType const type, Alloc *alloc);

Json bid_jgen_alias(BidAlias const alias, Alloc *alloc);

Json bid_jgen_method(BidMethod const method, Alloc *alloc);

Json bid_jgen_iface(BidIface const iface, Alloc *alloc);
