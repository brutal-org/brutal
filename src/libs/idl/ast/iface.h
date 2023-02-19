#pragma once

#include <idl/ast/types.h>
#include <brutal-ds>

typedef struct
{
    Str name;
    Vec(Str) args;
} IdlAttr;

typedef Vec(IdlAttr) IdlAttrs;

typedef struct _IdlAlias
{
    Str name;

    IdlType type;
    IdlAttrs attrs;
} IdlAlias;

typedef struct
{
    Str name;

    IdlType request;
    IdlType response;

    IdlAttrs attrs;
} IdlMethod;

typedef struct
{
    uint32_t id;
    Str name;
    IdlAttrs attrs;
    Vec(IdlMethod) methods;
} IdlIface;

typedef struct _IdlImport IdlImport;

typedef struct _IdlModule
{
    Str name;
    IdlAttrs attrs;

    Vec(IdlImport) imports;
    Vec(Str) includes;
    IdlType errors;
    Vec(IdlAlias) aliases;
    Vec(IdlIface) ifaces;
} IdlModule;

struct _IdlImport
{
    Str name;
    IdlModule module;
};
