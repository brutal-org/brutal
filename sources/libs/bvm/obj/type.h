#pragma once

#include <brutal/ds.h>
#include <bvm/obj/val.h>

typedef struct
{
    Str name;
    BvmType *type;
} BvmField;

typedef struct bvm_type
{
    Str name;

    BvmType *super;
    Vec(BvmField) fields;
    Vec(BvmField) vfields;
    Vec(BvmVal) vvals;
} BvmType;

BvmType *bvm_type_create(BvmType *super, Str name, Alloc *alloc);

void bvm_type_vfield(BvmType *self, Str name, BvmType *type, Alloc *alloc);

void bvm_type_field(BvmType *self, Str name, BvmType *type, Alloc *alloc);
