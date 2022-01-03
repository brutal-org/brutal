#pragma once
#include <brutal/base.h>
#include "brutal/text/str.h"

typedef enum
{
    UD_INFERRED,
    UD_GENERIC,
    UD_USER_DEFINED,
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

MAYBE_UNUSED static inline Str ud_type_to_str(UdTypeType type)
{
    switch (type)
    {
    case UD_GENERIC:
        return str$("Generic");
    case UD_INT:
    case UD_BOOL:
    case UD_STR:
        return str$("Builtin");
    case UD_USER_DEFINED:
        return str$("User defined");
    case UD_INFERRED:
        return str$("inferred");
    default:
        return str$("unknown");
    }

    return str$("unknown");
}

MAYBE_UNUSED static inline UdTypeType ud_str_to_type(Str str)
{

    if (str.buf[0] == '@')
    {
        return UD_GENERIC;
    }

    else if (str_eq(str, str$("Int")))
    {
        return UD_INT;
    }

    else if (str_eq(str, str$("Bool")))
    {
        return UD_BOOL;
    }

    else if (str_eq(str, str$("String")))
    {
        return UD_STR;
    }

    return UD_USER_DEFINED;
}
