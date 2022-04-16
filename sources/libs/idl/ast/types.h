#pragma once

#include <brutal-ds>

typedef struct _IdlType IdlType;

typedef struct
{
    Str name;
} IdlCtype;

typedef struct
{
    Str name;
    struct _IdlAlias *alias;
} IdlPrimitive;

typedef struct
{
    IdlType *subtype;
} IdlVec;

typedef struct _IdlEnumMember IdlEnumMember;

typedef struct
{
    Vec(IdlEnumMember) members;
} IdlEnum;

typedef struct _IdlStructMember IdlStructMember;

typedef struct
{
    Vec(IdlStructMember) members;
} IdlStruct;

typedef enum
{
    IDL_TYPE_NIL,

    IDL_TYPE_CTYPE,
    IDL_TYPE_PRIMITIVE,
    IDL_TYPE_ENUM,
    IDL_TYPE_STRUCT,
    IDL_TYPE_VEC,
} IdlTypeType;

struct _IdlType
{
    IdlTypeType type;

    union
    {
        IdlCtype ctype_;
        IdlPrimitive primitive_;
        IdlEnum enum_;
        IdlStruct struct_;
        IdlVec vec_;
    };
};

struct _IdlEnumMember
{
    Str name;
    int value;
};

struct _IdlStructMember
{
    Str name;
    IdlType type;
};
