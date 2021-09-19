#include <cc/ast/builder.h>

/* --- CVal  ---------------------------------------------------------------- */

CVal cval_signed(CSigned v)
{
    return (CVal){
        .type = CVAL_SIGNED,
        .signed_ = v,
    };
}

CVal cval_unsigned(CUnsigned v)
{
    return (CVal){
        .type = CVAL_SIGNED,
        .unsigned_ = v,
    };
}

CVal cval_float(double v)
{
    return (CVal){
        .type = CVAL_FLOAT,
        .float_ = v,
    };
}

CVal cval_string(Str str, Alloc *alloc)
{
    return (CVal){
        .type = CVAL_STRING,
        .string_ = str_dup(str, alloc),
    };
}
