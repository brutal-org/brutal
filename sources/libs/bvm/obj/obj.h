#pragma once

#include <bvm/obj/type.h>

struct bvm_obj
{
    BvmType *type;

    size_t vals_len;
    BvmVal *vals;

    size_t nvals_len;
    void *nvals;
};

BvmVal bvm_obj_create(BvmType *type, Alloc *alloc);

BvmVal bvm_obj_create_native(BvmType *type, void *data, size_t size, Alloc *alloc);

BvmVal bvm_obj_load(BvmVal val, size_t index);

BvmVal bvm_obj_loadv(BvmVal val, size_t index);

void bvm_obj_store(BvmVal val, size_t index, BvmVal data);

void bvm_obj_storev(BvmVal val, size_t index, BvmVal data);

BvmVal bvm_obj_isa(BvmVal lhs, BvmVal rhs);
