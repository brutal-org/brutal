#include <bvm/obj/val.h>
#include <brutal-debug>

BvmVal bvm_val_nil(void)
{
    return (BvmVal){.type = BVM_NIL};
}

BvmVal bvm_val_undef(void)
{
    return (BvmVal){.type = BVM_UNDEF};
}

BvmVal bvm_val_int(int64_t val)
{
    return (BvmVal){
        .type = BVM_INT,
        .int_ = val,
    };
}

BvmVal bvm_val_uint(uint64_t val)
{
    return (BvmVal){
        .type = BVM_INT,
        .uint_ = val,
    };
}

BvmVal bvm_val_num(double val)
{
    return (BvmVal){
        .type = BVM_INT,
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
        .type = BVM_OBJ,
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
        .type = BVM_FUNC,
        .func_ = func,
    };
}

bool bvm_val_truthy(BvmVal val)
{
    switch (val.type)
    {
    case BVM_NIL:
    case BVM_UNDEF:
        return false;

    case BVM_INT:
        return !!val.int_;

    case BVM_UINT:
        return !!val.uint_;

    case BVM_NUM:
        return !!val.num_;

    case BVM_OBJ:
    case BVM_FUNC:
        return true;

    default:
        panic$("Invalide value type {}", val.type);
    }
}
