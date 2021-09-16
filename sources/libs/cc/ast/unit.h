#pragma once

#include <cc/ast/decl.h>

typedef struct
{
    Str path;
    bool is_q_char; // q char = "hello"
} CInclude;

typedef struct
{
    Vec(Str) args;
} CPragma;

typedef enum
{
    CUNIT_NONE,
    CUNIT_INCLUDE,
    CUNIT_PRAGMA,
    CUNIT_DECLARATION
} CUnitType;

typedef struct
{
    CUnitType type;
    union
    {
        CInclude _include;
        CDecl _decl;
        CPragma _pragma;
    };
} CUnitEntry;

typedef struct
{
    Vec(CUnitEntry) units;
} CUnit;
