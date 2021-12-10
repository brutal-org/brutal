#include <brutal/debug.h>
#include <bvm/value.h>

BvmVal bvm_val_nil(void)
{
    return (BvmVal){.type = BVM_VAL_NIL};
}

BvmVal bvm_val_undef(void)
{
    return (BvmVal){.type = BVM_VAL_UNDEF};
}

BvmVal bvm_val_int(int64_t val)
{
    return (BvmVal){
        .type = BVM_VAL_INT,
        .int_ = val,
    };
}

BvmVal bvm_val_uint(uint64_t val)
{
    return (BvmVal){
        .type = BVM_VAL_INT,
        .uint_ = val,
    };
}

BvmVal bvm_val_num(double val)
{
    return (BvmVal){
        .type = BVM_VAL_INT,
        .num_ = val,
    };
}

BvmVal bvm_val_obj(BvmObj *obj)
{
    if (!obj)
    {
        return bvm_val_nil();
    }

    return (BvmVal){
        .type = BVM_VAL_OBJ,
        .obj_ = obj,
    };
}

BvmVal bvm_val_func(BvmFunc *func)
{
    if (!func)
    {
        return bvm_val_nil();
    }

    return (BvmVal){
        .type = BVM_VAL_FUNC,
        .func_ = func,
    };
}

BvmVal bvm_val_type(BvmType *type)
{
    if (!type)
    {
        return bvm_val_nil();
    }

    return (BvmVal){
        .type = BVM_VAL_TYPE,
        .type_ = type,
    };
}

bool bvm_val_truthy(BvmVal val)
{
    switch (val.type)
    {
    case BVM_VAL_NIL:
    case BVM_VAL_UNDEF:
        return false;

    case BVM_VAL_INT:
        return !!val.int_;

    case BVM_VAL_UINT:
        return !!val.uint_;

    case BVM_VAL_NUM:
        return !!val.num_;

    case BVM_VAL_OBJ:
    case BVM_VAL_FUNC:
    case BVM_VAL_TYPE:
        return true;

    default:
        panic$("Invalide value type {}", val.type);
    }
}

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
