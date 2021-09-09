#pragma once

#include <cc/ast/decl.h>

typedef struct
{
    Str path;
    bool is_q_char; // q char = "hello"
} CInclude;

typedef struct
{
    Vec(CInclude) includes;
    Vec(CDecl) declarations;
} CTransUnit;
