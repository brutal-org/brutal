#include <cc/builder/val.h>

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
        .type = CVAL_UNSIGNED,
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

CVal cval_string(Str str)
{
    return (CVal){
        .type = CVAL_STRING,
        .string_ = str,
    };
}
