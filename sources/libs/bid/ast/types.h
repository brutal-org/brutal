#pragma once

#include <brutal/ds.h>

typedef struct bid_type BidType;

typedef struct
{
    Str name;
    Str mangled;
} BidPrimitive;

typedef struct
{
    BidType *subtype;
} BidVec;

typedef struct bid_enum_member BidEnumMember;

typedef struct
{
    Vec(BidEnumMember) members;
} BidEnum;

typedef struct bid_struct_member BidStructMember;

typedef struct
{
    Vec(BidStructMember) members;
} BidStruct;

typedef enum
{
    BID_TYPE_NIL,

    BID_TYPE_PRIMITIVE,
    BID_TYPE_ENUM,
    BID_TYPE_STRUCT,
    BID_TYPE_VEC,
} BidTypeType;

struct bid_type
{
    BidTypeType type;

    union
    {
        BidPrimitive primitive_;
        BidEnum enum_;
        BidStruct struct_;
        BidVec vec_;
    };
};

struct bid_enum_member
{
    Str name;
    Str mangled;
};

struct bid_struct_member
{
    Str name;
    BidType type;
};
