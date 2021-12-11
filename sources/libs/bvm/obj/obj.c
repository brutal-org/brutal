#include <bvm/obj/obj.h>

BvmVal bvm_obj_create(BvmType *type, Alloc *alloc)
{
    return bvm_obj_create_native(type, nullptr, 0, alloc);
}

BvmVal bvm_obj_create_native(BvmType *type, void *data, size_t size, Alloc *alloc)
{
    size_t managed_size = type->fields.len * sizeof(BvmVal);

    BvmObj *obj = alloc_malloc(alloc, sizeof(BvmObj) + managed_size + size);
    obj->type = type;

    obj->vals = (BvmVal *)((char *)obj + sizeof(BvmObj));
    obj->vals_len = type->fields.len;

    obj->nvals = ((char *)obj + sizeof(BvmObj) + managed_size);
    obj->nvals_len = size;
    mem_cpy(obj->nvals, data, size);

    return bvm_val_obj(obj);
}

BvmVal bvm_obj_load(BvmVal val, size_t index)
{
    if (val.type != BVM_VAL_OBJ)
    {
        return bvm_val_nil();
    }

    BvmObj *obj = val.obj_;

    return obj->vals[index];
}

BvmVal bvm_obj_loadv(BvmVal val, size_t index)
{
    if (val.type != BVM_VAL_OBJ)
    {
        return bvm_val_nil();
    }

    BvmType *type = val.obj_->type;
    return bvm_type_loadv(type, index);
}

void bvm_obj_store(BvmVal val, size_t index, BvmVal data)
{
    if (val.type != BVM_VAL_OBJ)
    {
        return;
    }

    BvmObj *obj = val.obj_;

    obj->vals[index] = data;
}

void bvm_obj_storev(BvmVal val, size_t index, BvmVal data)
{
    if (val.type != BVM_VAL_OBJ)
    {
        return;
    }

    BvmType *type = val.obj_->type;

    vec_at(&type->vvals, index) = data;
}

BvmVal bvm_obj_isa(BvmVal lhs, BvmVal rhs)
{
    if (lhs.type != BVM_VAL_OBJ || rhs.type != BVM_VAL_TYPE)
    {
        return bvm_val_int(0);
    }

    BvmType *type = lhs.obj_->type;

    while (type)
    {
        if (lhs.obj_->type == rhs.type_)
        {
            return bvm_val_int(1);
        }

        type = type->super;
    }

    return bvm_val_int(0);
}
