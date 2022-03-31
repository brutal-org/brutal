#pragma once

#include <brutal/text/str.h>

typedef struct
{
    int line;
    Str func;
    Str file;
} Loc;

#define loc$() \
    (Loc) { __LINE__, str_const$(__func__), str_const$(__FILE__), }
