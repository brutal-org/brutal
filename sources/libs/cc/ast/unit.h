#pragma once

#include <cc/ast/decl.h>

typedef struct
{
    Str path;
    bool is_system;
} CInclude;

typedef struct
{
    Str text;
} CPragma;

typedef struct
{
    Str name;
    Vec(Str) args;
    CExpr expression;
} CDefine;

typedef enum
{
    CUNIT_INVALID,

    CUNIT_INCLUDE,
    CUNIT_PRAGMA,
    CUNIT_DECLARATION,
    CUNIT_DEFINE
} CUnitType;

typedef struct
{
    SrcRef ref;
    CUnitType type;

    union
    {
        CInclude _include;
        CDecl _decl;
        CPragma _pragma;
        CDefine _define;
    };
} CUnitEntry;

typedef struct
{
    Vec(CUnitEntry) units;
} CUnit;
