#pragma once

#include <bvm/eval/instr.h>
#include <bvm/obj/val.h>
#include <brutal-ds>

struct _BvmLocal;
struct _BvmFrame;
struct _BvmGlobal;

struct _BvmFunc
{
    bool native;

    union
    {
        BvmVal (*native_)(struct _BvmLocal *local, struct _BvmGlobal *mem);

        struct
        {
            size_t locals;
            Vec(BvmInstr) code;
        } managed_;
    };
};

BvmFunc *bvm_func_create(Alloc *alloc);

int bvm_func_local(BvmFunc *self);

uint64_t bvm_func_emit(BvmFunc *self, BvmOp op);

uint64_t bvm_func_emitu(BvmFunc *self, BvmOp op, uint64_t u);

uint64_t bvm_func_emiti(BvmFunc *self, BvmOp op, int64_t i);

uint64_t bvm_func_emitn(BvmFunc *self, BvmOp op, double n);

void bvm_func_jump(BvmFunc *self, uint64_t addr, uint64_t dest);
