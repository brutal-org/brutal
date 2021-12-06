#pragma once

#include <bvm/obj.h>

typedef struct
{
    Str name;
    BvmVal val;
} BvmSym;

typedef struct
{
    Vec(BvmSym) syms;
} BvmMod;