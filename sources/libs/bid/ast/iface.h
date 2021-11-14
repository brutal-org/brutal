#pragma once

#include <bid/ast/types.h>
#include <brutal/ds.h>

typedef struct
{
    Str name;
    Str mangled;

    BidType type;
} BidAlias;

typedef struct
{
    Str name;
    Str mangled;

    BidType request;
    BidType response;
} BidMethod;

typedef struct
{
    uint32_t id;
    Str name;

    BidType errors;
    Vec(BidAlias) aliases;
    Vec(BidMethod) methods;
} BidIface;
