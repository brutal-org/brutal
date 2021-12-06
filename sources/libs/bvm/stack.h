#pragma once

#include <bvm/obj.h>

typedef struct bvm_frame BvmFrame;

typedef struct bvm_stack BvmStack;

struct bvm_frame
{
    uintptr_t ret;
    BvmFrame *prev;

    int len;
    BvmVal vals[];
};

struct bvm_stack
{
    BvmFrame *curr;
    BvmVal *top;
};

void bvm_stack_init(BvmStack *stack, Alloc *alloc);

void bvm_stack_deinit(BvmStack *stack);

void bvm_stack_push(BvmStack *stack, BvmVal val);

BvmVal bvm_stack_pop(BvmStack *stack);

BvmFrame *bvm_stack_call(BvmStack *stack, uintptr_t ret, int narg);

uintptr_t bvm_stack_ret(BvmStack *stack, int nret);

BvmVal bvm_stack_load(BvmStack *stack, int index);

void bvm_stack_store(BvmStack *stack, int index, BvmVal val);
