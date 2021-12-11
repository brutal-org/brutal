#include <bvm/obj/obj.h>

BvmVal bvm_val_load(BvmVal val, size_t index)
{
    if (val.type != BVM_VAL_OBJ)
    {
        return bvm_val_nil();
    }

    BvmObj *obj = val.obj_;

    return obj->vals[index];
}

BvmVal bvm_val_loadv(BvmVal val, size_t index)
{
    if (val.type != BVM_VAL_OBJ)
    {
        return bvm_val_nil();
    }

    BvmType *type = val.obj_->type;

    return vec_at(&type->vvals, index);
}

void bvm_val_store(BvmVal val, size_t index, BvmVal data)
{
    if (val.type != BVM_VAL_OBJ)
    {
        return;
    }

    BvmObj *obj = val.obj_;

    obj->vals[index] = data;
}

void bvm_val_storev(BvmVal val, size_t index, BvmVal data)
{
    if (val.type != BVM_VAL_OBJ)
    {
        return;
    }

    BvmType *type = val.obj_->type;

    vec_at(&type->vvals, index) = data;
}

BvmVal bvm_val_isa(BvmVal lhs, BvmVal rhs)
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
