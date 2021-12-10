#pragma once

#include <brutal/ds.h>
#include <bvm/instr.h>

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

typedef struct
{
    Str name;
    BvmType *type;
} BvmField;

typedef struct
{
    BvmType *ret;
    Vec(BvmType *) args;
    bool variadic;
} BvmSig;

typedef struct bvm_type
{
    Str name;

    BvmType *super;
    Vec(BvmField) fields;
    Vec(BvmField) vfields;
    Vec(BvmVal) vvals;
} BvmType;

struct bvm_obj
{
    BvmType *type;

    size_t vals_len;
    BvmVal *vals;

    size_t nvals_len;
    void *nvals;
};

struct bvm_local;
struct bvm_frame;
struct bvm_mem;

struct bvm_func
{
    bool native;
    BvmSig *sig;

    union
    {
        BvmVal (*native_)(struct bvm_local *local, struct bvm_mem *mem);

        struct
        {
            size_t locals;
            Vec(BvmInstr) code;
        } managed_;
    };
};

BvmVal bvm_val_nil(void);

BvmVal bvm_val_undef(void);

BvmVal bvm_val_int(int64_t val);

BvmVal bvm_val_uint(uint64_t val);

BvmVal bvm_val_num(double val);

BvmVal bvm_val_obj(BvmObj *obj);

BvmVal bvm_val_func(BvmFunc *func);

BvmVal bvm_val_type(BvmType *type);

bool bvm_val_truthy(BvmVal val);

BvmVal bvm_val_load(BvmVal val, size_t index);

BvmVal bvm_val_loadv(BvmVal val, size_t index);

void bvm_val_store(BvmVal val, size_t index, BvmVal data);

void bvm_val_storev(BvmVal val, size_t index, BvmVal data);

BvmVal bvm_val_isa(BvmVal lhs, BvmVal rhs);
