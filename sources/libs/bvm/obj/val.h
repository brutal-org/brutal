#pragma once

#include <brutal/base.h>

typedef struct bvm_type BvmType;
typedef struct bvm_obj BvmObj;
typedef struct bvm_func BvmFunc;

typedef enum
{
    BVM_VAL_NIL,
    BVM_VAL_UNDEF,

    BVM_VAL_INT,
    BVM_VAL_UINT,
    BVM_VAL_NUM,

    BVM_VAL_OBJ,
    BVM_VAL_FUNC,
    BVM_VAL_TYPE,
} BvmValType;

typedef struct
{
    BvmValType type;

    union
    {
        int64_t int_;
        uint64_t uint_;
        double num_;

        BvmObj *obj_;
        BvmFunc *func_;
        BvmType *type_;
    };
} BvmVal;

BvmVal bvm_val_nil(void);

BvmVal bvm_val_undef(void);

BvmVal bvm_val_int(int64_t val);

BvmVal bvm_val_uint(uint64_t val);

BvmVal bvm_val_num(double val);

BvmVal bvm_val_obj(BvmObj *obj);

BvmVal bvm_val_func(BvmFunc *func);

BvmVal bvm_val_type(BvmType *type);

bool bvm_val_truthy(BvmVal val);
