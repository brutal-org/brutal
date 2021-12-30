#pragma once
#include <brutal/base.h>

typedef enum
{
    UD_INT,
    UD_BOOL,
    UD_STR,
    UD_FLOAT,
} UdTypeType;

typedef struct
{
    UdTypeType type;
    Str name;
} UdType;
