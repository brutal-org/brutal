#include <bvm/obj/obj.h>

BvmObj *bvm_obj_create(size_t size, BvmObj *proto, Alloc *alloc)
{
    size_t alloc_size = sizeof(BvmObj) + size * sizeof(BvmVal);

    BvmObj *self = alloc_malloc(alloc, alloc_size);
    self->proto = proto;
    self->len = proto->len;

    return self;
}

BvmVal bvm_obj_load(BvmObj *self, size_t index)
{
    if (index >= self->len)
    {
        return bvm_val_undef();
    }

    return self->vals[index];
}

BvmVal bvm_obj_loadv(BvmObj *self, size_t index)
{
    if (!self)
    {
        return bvm_val_undef();
    }

    if (index >= self->len)
    {
        return bvm_val_undef();
    }

    if (self->vals[index].type != BVM_UNDEF)
    {
        return self->vals[index];
    }

    return bvm_obj_loadv(self->proto, index);
}

void bvm_obj_store(BvmObj *self, size_t index, BvmVal data)
{
    if (index >= self->len)
    {
        return;
    }

    self->vals[index] = data;
}

void bvm_obj_storev(BvmObj *self, size_t index, BvmVal data)
{
    if (!self)
    {
        return;
    }

    if (index >= self->len)
    {
        return;
    }

    if (self->vals[index].type != BVM_UNDEF)
    {
        self->vals[index] = data;
    }

    bvm_obj_storev(self->proto, index, data);
}

bool bvm_obj_isa(BvmObj *self, BvmObj *proto)
{
    if (!self)
    {
        return false;
    }

    if (self->proto == proto)
    {
        return true;
    }

    return bvm_obj_isa(self->proto, proto);
}
