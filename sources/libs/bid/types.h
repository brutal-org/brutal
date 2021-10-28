#pragma once

#include <brutal/ds.h>

typedef struct bid_type BidType;

typedef struct
{
    Str name;
    Vec(Str) args;
} BidPrimitive;

typedef struct
{
    Vec(Str) members;
} BidEnum;

typedef struct bid_member BidMember;

typedef struct
{
    Vec(BidMember) members;
} BidStruct;

typedef enum
{
    BID_TYPE_NONE,

    BID_TYPE_PRIMITIVE,
    BID_TYPE_ENUM,
    BID_TYPE_STRUCT,
} BidTypeType;

struct bid_type
{
    BidTypeType type;

    union
    {
        BidPrimitive primitive_;
        BidEnum enum_;
        BidStruct struct_;
    };
};

struct bid_member
{
    Str name;
    BidType type;
    bool kernel_handle;
};
