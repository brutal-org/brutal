#include <bvm/ctx/mem.h>
#include <bvm/obj/obj.h>
#include <bvm/obj/type.h>

void bvm_mem_init(BvmMem *self, Alloc *alloc)
{
    self->alloc = alloc;
    vec_init(&self->data, alloc);
}

void bvm_mem_deinit(BvmMem *self)
{
    vec_deinit(&self->data);
}

BvmVal bvm_mem_load(BvmMem *self, int index)
{
    return vec_at(&self->data, index).val;
}

void bvm_mem_store(BvmMem *self, int index, BvmVal val)
{
    vec_at(&self->data, index).val = val;
}

int bvm_mem_add(BvmMem *self, Str name, BvmVal val)
{
    BvmSym sym = {str_dup(name, self->alloc), val};
    vec_push(&self->data, sym);
    return self->data.len - 1;
}

BvmVal bvm_mem_new(BvmMem *self, BvmType *type)
{
    return bvm_mem_new_native(self, type, nullptr, 0);
}

BvmVal bvm_mem_new_native(BvmMem *self, BvmType *type, void *data, size_t size)
{
    size_t managed_size = type->fields.len * sizeof(BvmVal);

    BvmObj *obj = alloc_malloc(self->alloc, sizeof(BvmObj) + managed_size + size);
    obj->type = type;

    obj->vals = (BvmVal *)((char *)obj + sizeof(BvmObj));
    obj->vals_len = type->fields.len;

    obj->nvals = ((char *)obj + sizeof(BvmObj) + managed_size);
    obj->nvals_len = size;
    mem_cpy(obj->nvals, data, size);

    return bvm_val_obj(obj);
}
