#pragma once

#include <brutal/text.h>

typedef struct
{
    Str name;
    Str cname;
    Str pack_fn;
    Str unpack_fn;
} BidBuiltinType;

BidBuiltinType *bid_lookup_builtin(Str name);
