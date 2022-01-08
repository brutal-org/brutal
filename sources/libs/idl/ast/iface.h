#pragma once

#include <brutal/ds.h>
#include <idl/ast/types.h>

typedef struct
{
    Str name;
    Str mangled;

    IdlType type;
} IdlAlias;

typedef struct
{
    Str name;
    Str mangled;

    IdlType request;
    IdlType response;
} IdlMethod;

typedef struct
{
    uint32_t id;
    Str name;

    IdlType errors;
    Vec(IdlAlias) aliases;
    Vec(IdlMethod) methods;
} IdlIface;
