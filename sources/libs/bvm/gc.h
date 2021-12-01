#pragma once

#include <brutal/alloc.h>
#include <bvm/obj.h>

typedef struct
{
    Alloc *alloc;
} BvmGc;

void bvm_gc_init(BvmGc *self, Alloc *alloc);

void bvm_gc_deinit(BvmGc *self);

void bvm_gc_begin(BvmGc *self);

void bvm_gc_mark(BvmGc *self, BvmObj *obj);

void bvm_gc_end(BvmGc *self);
