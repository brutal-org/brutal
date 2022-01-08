#pragma once

#include <brutal/text.h>

typedef struct
{
    Str name;
    Str cname;
    Str pack_fn;
    Str unpack_fn;
} IdlBuiltinType;

IdlBuiltinType *idl_lookup_builtin(Str name);
