#pragma once

#include <bvm/instr.h>
#include <bvm/value.h>

typedef struct
{
    Str name;
    BvmVal val;
} BvmSym;

typedef struct bvm_mem
{
    Alloc *alloc;
    Vec(BvmInstr) code;
    Vec(BvmSym) data;
} BvmMem;

void bvm_mem_init(BvmMem *self, Alloc *alloc);

void bvm_mem_deinit(BvmMem *self);

BvmVal bvm_mem_load(BvmMem *self, int index);

void bvm_mem_store(BvmMem *self, int index, BvmVal val);

BvmInstr bvm_mem_ifetch(BvmMem *self, size_t ip);
