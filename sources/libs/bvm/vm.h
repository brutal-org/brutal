#pragma once

#include <bvm/gc.h>

typedef struct bvm
{
    BvmGc *gc;
} Bvm;

void bvm_init(Bvm *self, BvmGc *gc);

void bvm_deinit(Bvm *self);

bool bvm_eval(Bvm *self);
