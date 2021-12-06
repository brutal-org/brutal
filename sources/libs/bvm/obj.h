#pragma once

#include <brutal/ds.h>

typedef enum bvm_val_type BvmValType;
typedef struct bvm_val BvmVal;

typedef struct bvm_field BvmField;
typedef struct bvm_method BvmMethod;

typedef struct bvm_type BvmType;
typedef struct bvm_obj BvmObj;

typedef struct bvm_sig BvmSig;

typedef enum bvm_func_type BvmFuncType;
typedef struct bvm_func BvmFunc;

enum bvm_val_type
{
    BVM_VAL_NIL,

    BVM_VAL_BOOL,

    BVM_VAL_ISIZE,
    BVM_VAL_USIZE,

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
};

struct bvm_val
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
};

struct bvm_field
{
    Str name;
    BvmType *type_;
};

struct bvm_method
{
    Str name;
    BvmSig *sig;
};

struct bvm_type
{
    Str name;

    Vec(BvmType) extends;
    Vec(BvmField) fields;
    Vec(BvmMethod) methods;
};

struct bvm_obj
{
    uint64_t gen;
    BvmType *type;
    Vec(BvmField) fields;
};

struct bvm_sig
{
    BvmType *ret;
    Vec(BvmType *) args;
};

enum bvm_func_type
{
    BVM_FUNC_TYPE_NIL,

    BVM_FUNC_TYPE_NATIVE,
    BVM_FUNC_TYPE_MANAGED,
};

struct bvm_func
{
    bool native;
    BvmSig sig;

    union
    {
        BvmVal (*native_)(Bvm *vm, BvmFrame *frame);
        uintptr_t managed_;
    };
};
