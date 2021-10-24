#pragma once

#include <brutal/base.h>
#include <brutal/debug.h>
#include <brutal/ds.h>
#include <cc/ast/val.h>

typedef enum
{
    CTYPE_INVALID,

    CTYPE_VOID,
    CTYPE_AUTO,
    CTYPE_BOOL,
    CTYPE_PTR,
    CTYPE_PARENT,
    CTYPE_ARRAY,
    CTYPE_SIGNED,
    CTYPE_UNSIGNED,
    CTYPE_FLOAT,
    CTYPE_STRUCT,
    CTYPE_UNION,
    CTYPE_ENUM,
    CTYPE_FUNC,
    CTYPE_NAME,
} CTypeType;

typedef enum
{
    CTYPE_CONST = (1 << 0),
    CTYPE_RESTRICT = (1 << 1),
    CTYPE_VOLATILE = (1 << 2),
} CTypeAttr;

typedef struct ctype CType;
typedef struct ctype_member CTypeMember;
typedef struct ctype_constant CTypeConstant;

struct ctype
{
    CTypeType type;
    CTypeAttr attr;
    Str name;

    union
    {
        struct
        {
            int precision;
        } signed_, unsigned_, float_;

        struct
        {
            CType *subtype;
        } ptr_, parent_;

        struct
        {
            CType *subtype;
            int size;
        } array_;

        struct
        {
            Vec(CTypeMember) members;
        } struct_, union_;

        struct
        {
            Vec(CTypeConstant) constants;
        } enum_;

        struct
        {
            CType *ret;
            Vec(CTypeMember) params;
        } func_;
    };
};

struct ctype_member
{
    CType type;
    Str name;
};

struct ctype_constant
{
    Str name;
    CVal value;
};

typedef Slice(CType) CTypeSlice;

Str ctype_to_str(CTypeType type);
