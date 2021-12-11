#include <brutal/debug.h>
#include <bvm/obj/val.h>

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
