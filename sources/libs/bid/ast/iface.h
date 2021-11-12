#pragma once

#include <bid/ast/types.h>
#include <brutal/ds.h>

typedef struct
{
    Str name;
    BidType type;
} BidAlias;

typedef struct
{
    Str name;

    BidType request;
    BidType response;
} BidMethod;

typedef struct
{
    Str name;
    uint32_t id;

    BidType errors;
    Vec(BidAlias) aliases;
    Vec(BidMethod) methods;
} BidIface;
