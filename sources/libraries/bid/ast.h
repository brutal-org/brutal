#pragma once

#include <bid/bid.h>
#include <brutal/ds.h>

typedef struct bid_type BidType;

typedef struct
{
    Str name;
    Vec(BidType) params;
} BidGeneric;

typedef struct
{
    Vec(Str) members;
} BidEnum;

typedef struct bid_var BidVar;

typedef struct
{
    Vec(BidVar) members;
} BidStruct;

typedef enum
{
    BID_TYPE_NONE,

    BID_TYPE_GENERIC,
    BID_TYPE_ENUM,
    BID_TYPE_STRUCT,
} BidTypeType;

struct bid_type
{
    BidTypeType type;

    union
    {
        BidGeneric generic_;
        BidEnum enum_;
        BidStruct struct_;
    };
};

struct bid_var
{
    Str name;
    BidType type;
};

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
    BidType data;
} BidEvent;

typedef struct
{
    Str name;

    BidEnum errors;
    Vec(BidAlias) aliases;
    Vec(BidEvent) events;
    Vec(BidMethod) methodes;
} BidInterface;
