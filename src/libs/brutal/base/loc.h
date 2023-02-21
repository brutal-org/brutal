#pragma once

#include "str.h"

typedef struct
{
    int line;
    Str func;
    Str file;
} Loc;

#define loc$()                        \
    (Loc const)                       \
    {                                 \
        .line = __LINE__,             \
        .func = str_const$(__func__), \
        .file = str_const$(__FILE__), \
    }

#define loc_const$()                  \
    {                                 \
        .line = __LINE__,             \
        .func = str_const$(__func__), \
        .file = str_const$(__FILE__), \
    }
