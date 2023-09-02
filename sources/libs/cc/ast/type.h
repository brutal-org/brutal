#pragma once

#include <cc/ast/val.h>
#include <brutal-ds>

typedef enum
{
    CTYPE_INVALID,

    CTYPE_TAIL,

    CTYPE_VOID,
    CTYPE_AUTO,
    CTYPE_BOOL,
    CTYPE_PTR,
    CTYPE_REF,
    CTYPE_PARENT,
    CTYPE_ARRAY,

    CTYPE_CHAR,
    CTYPE_SHORT,
    CTYPE_INT,
    CTYPE_LONG,
    CTYPE_LONGLONG,

    CTYPE_UCHAR,
    CTYPE_USHORT,
    CTYPE_UINT,
    CTYPE_ULONG,
    CTYPE_ULONGLONG,

    CTYPE_FLOAT,
    CTYPE_DOUBLE,

    CTYPE_STRUCT,
    CTYPE_UNION,
    CTYPE_ENUM,
    CTYPE_FUNC,
    CTYPE_NAME,

    CTYPE_COUNT,
} CTypeType;

typedef enum
{
    CTYPE_CONST = (1 << 0),
    CTYPE_RESTRICT = (1 << 1),
    CTYPE_VOLATILE = (1 << 2),
} CTypeAttr;

#define CTYPE_ARRAY_UNBOUNDED (-1)

typedef struct _CType CType;
typedef struct _CTypeMember CTypeMember;
typedef struct _CTypeConst CTypeConst;
typedef Vec(CTypeMember) CTypeMembers;

struct _CType
{
    SrcRef ref;
    CTypeType type;
    CTypeAttr attr;
    Str name;

    union
    {
        struct
        {
            CType *subtype;
        } ptr_, ref_, parent_;

        struct
        {
            CType *subtype;
            int size;
        } array_;

        struct
        {
            CTypeMembers members;
        } struct_, union_;

        struct
        {
            Vec(CTypeConst) constants;
        } enum_;

        struct
        {
            CType *ret;
            CTypeMembers params;
        } func_;
    };
};

struct _CTypeMember
{
    CType type;
    Str name;
};

struct _CTypeConst
{
    Str name;
    CVal value;
};
