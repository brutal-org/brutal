#pragma once

#include <brutal/ds.h>
#include <bvm/eval/instr.h>
#include <bvm/obj/val.h>

typedef struct
{
    Str name;
    BvmVal val;
} BvmSym;

typedef struct bvm_mem
{
    Alloc *alloc;
    Vec(BvmSym) data;
} BvmMem;

void bvm_mem_init(BvmMem *self, Alloc *alloc);

void bvm_mem_deinit(BvmMem *self);

BvmVal bvm_mem_load(BvmMem *self, int index);

void bvm_mem_store(BvmMem *self, int index, BvmVal val);

int bvm_mem_add(BvmMem *self, Str name, BvmVal val);

BvmVal bvm_mem_new(BvmMem *self, BvmType *type);

BvmVal bvm_mem_new_native(BvmMem *self, BvmType *type, void *data, size_t size);
