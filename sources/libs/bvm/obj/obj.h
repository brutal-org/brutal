#pragma once

#include <brutal-alloc>
#include <bvm/obj/val.h>

struct _BvmObj
{
    BvmObj *proto;
    size_t len;
    BvmVal vals[];
};

BvmObj *bvm_obj_create(size_t size, BvmObj *proto, Alloc *alloc);

BvmVal bvm_obj_load(BvmObj *self, size_t index);

BvmVal bvm_obj_loadv(BvmObj *self, size_t index);

void bvm_obj_store(BvmObj *self, size_t index, BvmVal data);

void bvm_obj_storev(BvmObj *self, size_t index, BvmVal data);

bool bvm_obj_isa(BvmObj *self, BvmObj *proto);
