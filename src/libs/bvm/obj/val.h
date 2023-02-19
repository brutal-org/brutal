#pragma once

#include <brutal-base>

typedef struct _BvmObj BvmObj;
typedef struct _BvmFunc BvmFunc;

typedef enum
{
    BVM_UNDEF,
    BVM_NIL,
    BVM_INT,
    BVM_UINT,
    BVM_NUM,
    BVM_OBJ,
    BVM_FUNC,
} BvmType;

typedef struct
{
    BvmType type;

    union
    {
        int64_t int_;
        uint64_t uint_;
        double num_;

        BvmObj *obj_;
        BvmFunc *func_;
    };
} BvmVal;

BvmVal bvm_val_nil(void);

BvmVal bvm_val_undef(void);

BvmVal bvm_val_int(int64_t val);

BvmVal bvm_val_uint(uint64_t val);

BvmVal bvm_val_num(double val);

BvmVal bvm_val_obj(BvmObj *obj);

BvmVal bvm_val_func(BvmFunc *func);

bool bvm_val_truthy(BvmVal val);
