#include <bvm/ctx/global.h>
#include <bvm/obj/obj.h>

void bvm_global_init(BvmGlobal *self, Alloc *alloc)
{
    vec_init(&self->data, alloc);
}

void bvm_global_deinit(BvmGlobal *self)
{
    vec_deinit(&self->data);
}

BvmVal bvm_global_load(BvmGlobal *self, int index)
{
    return vec_at(&self->data, index).val;
}

void bvm_global_store(BvmGlobal *self, int index, BvmVal val)
{
    vec_at(&self->data, index).val = val;
}

int bvm_global_add(BvmGlobal *self, Str name, BvmVal val)
{
    BvmSym sym = {str_dup(name, self->alloc), val};
    vec_push(&self->data, sym);
    return self->data.len - 1;
}
