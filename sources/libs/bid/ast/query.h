#pragma once

#include <bid/ast/iface.h>
#include <bid/ast/types.h>

bool type_in_interface(BidIface const *interface, Str name);

bool bid_type_is_handle(BidType *type);
