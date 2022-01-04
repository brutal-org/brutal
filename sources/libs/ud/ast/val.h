#pragma once
#include <brutal/text.h>

typedef enum
{
    UD_VAL_INT,
    UD_VAL_STR,
    UD_VAL_FLOAT,

} UdValType;

typedef struct
{

    UdValType type;

    union
    {
        Str string;
        uint64_t int_;
    };

} UdVal;
