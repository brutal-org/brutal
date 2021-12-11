#include <bvm/obj/obj.h>

BvmType *bvm_type_create(BvmType *super, Str name, Alloc *alloc)
{
    BvmType *self = alloc_make(alloc, BvmType);

    self->super = super;
    self->name = name;

    vec_init(&self->fields, alloc);
    vec_init(&self->vfields, alloc);

    vec_init(&self->vvals, alloc);

    vec_foreach_v(val, &super->vvals)
    {
        vec_push(&self->vvals, val);
    }

    return self;
}

void bvm_type_vfield(BvmType *self, Str name, BvmType *type, BvmVal val)
{
    BvmField field = {name, type};
    vec_push(&self->vfields, field);
    vec_push(&self->vvals, val);
}

void bvm_type_field(BvmType *self, Str name, BvmType *type)
{
    BvmField field = {name, type};
    vec_push(&self->fields, field);
}

BvmVal bvm_type_loadv(BvmType *self, size_t index)
{
    return vec_at(&self->vvals, index);
}

void bvm_type_storev(BvmType *self, size_t index, BvmVal data)
{
    vec_at(&self->vvals, index) = data;
}
