#include <bvm/obj/obj.h>

BvmType *bvm_type_create(BvmType *super, Str name, Alloc *alloc)
{
    BvmType *self = alloc_make(alloc, BvmType);

    self->super = super;
    self->name = str_dup(name, alloc);

    vec_init(&self->fields, alloc);
    vec_init(&self->vfields, alloc);
    vec_init(&self->vfields, alloc);

    return self;
}

void bvm_type_vfield(BvmType *self, Str name, BvmType *type, Alloc *alloc)
{
    BvmField field = {str_dup(name, alloc), type};
    vec_push(&self->vfields, field);
}

void bvm_type_field(BvmType *self, Str name, BvmType *type, Alloc *alloc)
{
    BvmField field = {str_dup(name, alloc), type};
    vec_push(&self->fields, field);
}
