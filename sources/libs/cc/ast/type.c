#include <cc/ast/type.h>

const char *ctype_type_to_str[CTYPE_COUNT] = {
    [CTYPE_INVALID] = "invalid",

    [CTYPE_ERROR] = "error",
    [CTYPE_TAIL] = "tail",

    [CTYPE_VOID] = "void",
    [CTYPE_AUTO] = "auto",
    [CTYPE_BOOL] = "bool",
    [CTYPE_PTR] = "",
    [CTYPE_ARRAY] = "",
    [CTYPE_SIGNED] = "signed",
    [CTYPE_UNSIGNED] = "unsigned",
    [CTYPE_FLOAT] = "float",
    [CTYPE_STRUCT] = "struct",
    [CTYPE_UNION] = "union",
    [CTYPE_ENUM] = "enum",
};

Str ctype_to_str(CTypeType type)
{
    if (ctype_type_to_str[type])
    {
        return str$(ctype_type_to_str[type]);
    }
    else
    {
        return str$("");
    }
}
