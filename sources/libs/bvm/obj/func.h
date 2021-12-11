#pragma once

#include <bvm/eval/instr.h>
#include <bvm/obj/type.h>

struct bvm_local;
struct bvm_frame;
struct bvm_mem;

typedef struct
{
    BvmType *ret;
    Vec(BvmType *) args;
    bool variadic;
} BvmSig;

BvmSig *bvm_sig_create(BvmType *ret, bool variadic, Alloc *alloc);

void bvm_sig_arg(BvmSig *self, BvmType *type);

struct bvm_func
{
    bool native;
    BvmSig *sig;

    union
    {
        BvmVal (*native_)(struct bvm_local *local, struct bvm_mem *mem);

        struct
        {
            size_t locals;
            Vec(BvmInstr) code;
        } managed_;
    };
};

BvmFunc *bvm_func_create(BvmSig *sig, Alloc *alloc);

int bvm_func_local(BvmFunc *self);

uint64_t bvm_func_emit(BvmFunc *self, BvmOp op);

uint64_t bvm_func_emitu(BvmFunc *self, BvmOp op, uint64_t u);

uint64_t bvm_func_emiti(BvmFunc *self, BvmOp op, int64_t i);

uint64_t bvm_func_emitn(BvmFunc *self, BvmOp op, double n);

void bvm_func_jump(BvmFunc *self, uint64_t addr, uint64_t dest);
