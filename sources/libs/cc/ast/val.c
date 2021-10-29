#include <cc/ast/val.h>

const char *cval_type_to_str_raw[CVAL_COUNT] =
    {
        [CVAL_SIGNED] = "signed",
        [CVAL_UNSIGNED] = "unsigned",
        [CVAL_FLOAT] = "float",
        [CVAL_STRING] = "string",
};

Str cval_type_to_str(CValType type)
{
    return str$(cval_type_to_str_raw[type]);
}
