#pragma once

#include <bruwutal/ds.h>
#include <bvm/eval/instr.h>
#include <bvm/obj/val.h>

typedef struct
{
    Str name;
    BvmVal val;
} BvmSym;

typedef struct _BvmGlobal
{
    Alloc *alloc;
    Vec(BvmSym) data;
} BvmGlobal;

void bvm_global_init(BvmGlobal *self, Alloc *alloc);

void bvm_global_deinit(BvmGlobal *self);

BvmVal bvm_global_load(BvmGlobal *self, int index);

void bvm_global_store(BvmGlobal *self, int index, BvmVal val);

int bvm_global_add(BvmGlobal *self, Str name, BvmVal val);
