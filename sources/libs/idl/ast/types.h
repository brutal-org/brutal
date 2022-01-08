#pragma once

#include <brutal/ds.h>

typedef struct idl_type IdlType;

typedef struct
{
    Str name;
    Str mangled;
} IdlPrimitive;

typedef struct
{
    IdlType *subtype;
} IdlVec;

typedef struct idl_enum_member IdlEnumMember;

typedef struct
{
    Vec(IdlEnumMember) members;
} IdlEnum;

typedef struct idl_struct_member IdlStructMember;

typedef struct
{
    Vec(IdlStructMember) members;
} IdlStruct;

typedef enum
{
    IDL_TYPE_NIL,

    IDL_TYPE_PRIMITIVE,
    IDL_TYPE_ENUM,
    IDL_TYPE_STRUCT,
    IDL_TYPE_VEC,
} IdlTypeType;

struct idl_type
{
    IdlTypeType type;

    union
    {
        IdlPrimitive primitive_;
        IdlEnum enum_;
        IdlStruct struct_;
        IdlVec vec_;
    };
};

struct idl_enum_member
{
    Str name;
    Str mangled;
};

struct idl_struct_member
{
    Str name;
    IdlType type;
};
