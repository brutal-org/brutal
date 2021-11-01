#pragma once

#include <bid/ast.h>
#include <json/objects.h>

Json bidgen_json_primitive(BidPrimitive primitive_, Alloc *alloc);

Json bidgen_json_enum(BidEnum enum_, Alloc *alloc);

Json bidgen_json_struct(BidStruct struct_, Alloc *alloc);

Json bidgen_json_type(BidType const type, Alloc *alloc);

Json bidgen_json_alias(BidAlias const alias, Alloc *alloc);

Json bidgen_json_event(BidEvent const event, Alloc *alloc);

Json bidgen_json_methode(BidMethod const methode, Alloc *alloc);

Json bidgen_json_iface(BidIface const iface, Alloc *alloc);
