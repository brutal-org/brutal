#pragma once

#include <brutal/ds.h>

typedef struct bvm_type BvmType;
typedef struct bvm_obj BvmObj;
typedef struct bvm_func BvmFunc;

typedef enum
{
    BVM_VAL_NIL,
    BVM_VAL_UNDEF,
    BVM_VAL_BOOL,

    BVM_VAL_I8,
    BVM_VAL_U8,
    BVM_VAL_I16,
    BVM_VAL_U16,
    BVM_VAL_I32,
    BVM_VAL_U32,
    BVM_VAL_I64,
    BVM_VAL_U64,

    BVM_VAL_F32,
    BVM_VAL_F64,

    BVM_VAL_OBJ,
    BVM_VAL_FUNC,
    BVM_VAL_TYPE,
    BVM_VAL_SIGN,
} BvmValType;

typedef struct
{
    BvmValType type;

    union
    {
        bool bool_;

        int8_t i8_;
        uint8_t u8_;
        int16_t i16_;
        uint16_t u16_;
        int32_t i32_;
        uint32_t u32_;
        int64_t i64_;
        uint64_t u64_;

        float f32_;
        double f64_;

        BvmObj *obj_;
        BvmFunc *func_;
        BvmType *type_;
    };
} BvmVal;

typedef struct
{
    Str name;
    BvmType *type_;
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
    uint64_t gen;
    BvmType *type;

    size_t nvals_len;
    uint8_t *nvals;

    size_t vals_len;
    BvmVal vals[];
};

struct bvm_local;
struct bvm_frame;
struct bvm_mem;

struct bvm_func
{
    bool native;
    BvmSig sig;

    union
    {
        BvmVal (*native_)(struct bvm_local *local, struct bvm_frame *frame, struct bvm_mem *mem);

        struct
        {
            uintptr_t entry;
            size_t size;
        } managed_;
    };
};

bool bvm_val_truthy(BvmVal val);

BvmVal bvm_val_load(BvmVal val, size_t index);
BvmVal bvm_val_loadv(BvmVal val, size_t index);
void bvm_val_store(BvmVal val, size_t index, BvmVal data);
void bvm_val_storev(BvmVal val, size_t index, BvmVal data);

BvmVal bvm_val_add(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_sub(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_div(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_mul(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_mod(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_neg(BvmVal lhs);

BvmVal bvm_val_gt(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_eq(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_geq(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_neq(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_isa(BvmVal lhs, BvmVal rhs);

BvmVal bvm_val_and(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_or(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_not(BvmVal lhs);

BvmVal bvm_val_lshift(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_rshift(BvmVal lhs, BvmVal rhs);

BvmVal bvm_val_band(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_bor(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_bxor(BvmVal lhs, BvmVal rhs);
BvmVal bvm_val_bnot(BvmVal lhs);