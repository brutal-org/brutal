#pragma once

#include <brutal-base>
#include <cc/ast/ref.h>

typedef int64_t CSigned;
typedef uint64_t CUnsigned;

typedef enum
{
    CVAL_INVALID,

    CVAL_CHAR,
    CVAL_SHORT,
    CVAL_INT,
    CVAL_LONG,
    CVAL_LONGLONG,

    CVAL_UCHAR,
    CVAL_USHORT,
    CVAL_UINT,
    CVAL_ULONG,
    CVAL_ULONGLONG,

    CVAL_FLOAT,
    CVAL_DOUBLE,

    CVAL_STRING,
    CVAL_COUNT,
} CValType;

typedef struct _CVal
{
    SrcRef ref;
    CValType type;

    union
    {
        CSigned signed_;
        CUnsigned unsigned_;
        double float_;
        Str str_;
    };
} CVal;
