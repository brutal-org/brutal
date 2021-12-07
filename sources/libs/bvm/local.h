#pragma once

#include <bvm/mem.h>

typedef struct bvm_frame
{
    struct bvm_frame *prev;

    size_t ret;

    size_t len;
    BvmVal vals[];
} BvmFrame;

typedef struct bvm_local
{
    size_t ip;
    size_t sp;

    BvmFrame *top;
    BvmFrame *bottom;
    size_t len;
} BvmLocal;

void bvm_local_init(BvmLocal *self, BvmMem *mem);

void bvm_local_deinit(BvmLocal *self);

void bvm_local_push(BvmLocal *self, BvmVal val);

BvmVal bvm_local_pop(BvmLocal *self);

BvmFrame *bvm_local_call(BvmLocal *self, int argc);

uintptr_t bvm_local_ret(BvmLocal *self);

BvmVal bvm_local_load(BvmLocal *self, int index);

void bvm_local_store(BvmLocal *self, int index, BvmVal val);
