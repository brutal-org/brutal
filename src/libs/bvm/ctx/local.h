#pragma once

#include <bvm/base/res.h>
#include <bvm/ctx/global.h>

typedef struct _BvmFrame
{
    uintptr_t ip;

    BvmVal *args;
    size_t argc;

    BvmVal *vars;
    size_t varc;

    BvmFunc *func;
} BvmFrame;

#define BVM_MAX_STACK (4096)
#define BVM_MAX_FRAME (128)

typedef struct _BvmLocal
{
    Alloc *alloc;

    BvmVal *stack;
    BvmVal *stack_top;

    BvmFrame *frame;
    BvmFrame *frame_curr;
} BvmLocal;

void bvm_local_init(BvmLocal *self, Alloc *alloc);

void bvm_local_deinit(BvmLocal *self);

void bvm_local_push(BvmLocal *self, BvmVal val);

BvmVal bvm_local_pop(BvmLocal *self);

BvmFrame *bvm_local_call(BvmLocal *self, BvmFunc *func, int argc);

BvmRes bvm_local_ret(BvmLocal *self, BvmVal val);

BvmVal bvm_local_load_var(BvmLocal *self, int index);

void bvm_local_store_var(BvmLocal *self, int index, BvmVal val);

BvmVal bvm_local_load_arg(BvmLocal *self, int index);

void bvm_local_store_arg(BvmLocal *self, int index, BvmVal val);

BvmInstr bvm_local_ifetch(BvmLocal *self);

void bvm_local_jump(BvmLocal *self, uint64_t addr);

void bvm_local_dump(BvmLocal *self);

void bvm_local_dump(BvmLocal *self);
