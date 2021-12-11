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

BvmVal bvm_val_load(BvmVal val, size_t index);

BvmVal bvm_val_loadv(BvmVal val, size_t index);

void bvm_val_store(BvmVal val, size_t index, BvmVal data);

void bvm_val_storev(BvmVal val, size_t index, BvmVal data);

BvmVal bvm_val_isa(BvmVal lhs, BvmVal rhs);
