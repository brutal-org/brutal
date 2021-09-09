#pragma once

#include <brutal/base.h>

typedef int CSigned;
typedef unsigned int CUnsigned;

typedef enum
{
    CVAL_INVALID,

    CVAL_SIGNED,
    CVAL_UNSIGNED,
    CVAL_FLOAT,
    CVAL_STRING,
} CValType;

typedef struct
{
    CValType type;

    union
    {
        CSigned signed_;
        CUnsigned unsigned_;
        double float_;
        Str string_;
    };
} CVal;
