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

BvmVal bvm_mod_load(BvmMod *mod, int index);

void bvm_mod_store(BvmMod *mod, int index, BvmVal val);
