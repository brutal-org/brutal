#include <brutal-debug>
#include <cc/builder/type.h>
#include <cc/builder/val.h>

static const char *cval_type_to_str_raw[CVAL_COUNT] = {
    [CVAL_INVALID] = "invalid",
    [CVAL_CHAR] = "char",
    [CVAL_SHORT] = "short",
    [CVAL_INT] = "int",
    [CVAL_LONG] = "long",
    [CVAL_LONGLONG] = "long long",
    [CVAL_UCHAR] = "unsigned char",
    [CVAL_USHORT] = "unsigned short",
    [CVAL_UINT] = "unsigned int",
    [CVAL_ULONG] = "unsigned long",
    [CVAL_ULONGLONG] = "unsigned long long",
    [CVAL_FLOAT] = "float",
    [CVAL_DOUBLE] = "double",
    [CVAL_STRING] = "string",
};

Str cval_to_str(CVal val)
{
    return str$(cval_type_to_str_raw[val.type]);
}

CType cval_type(CVal value, Alloc *alloc)
{
    switch (value.type)
    {
    case CVAL_FLOAT:
        return ctype_float();

    case CVAL_CHAR:
        return ctype_char();

    case CVAL_SHORT:
        return ctype_short();

    case CVAL_INT:
        return ctype_int();

    case CVAL_LONG:
        return ctype_long();

    case CVAL_LONGLONG:
        return ctype_longlong();

    case CVAL_UCHAR:
        return ctype_uchar();

    case CVAL_USHORT:
        return ctype_ushort();

    case CVAL_UINT:
        return ctype_uint();

    case CVAL_ULONG:
        return ctype_ulong();

    case CVAL_ULONGLONG:
        return ctype_ulonglong();

    case CVAL_STRING:
        return ctype_array(ctype_char(), value.str_.len, alloc);

    default:
        panic$("invalid cval type");
    }
}

/* --- Builder -------------------------------------------------------------- */

CVal cval_error(void)
{
    return (CVal){
        .type = CVAL_INVALID,
    };
}

CVal cval_char(CSigned val)
{
    return (CVal){
        .type = CVAL_CHAR,
        .signed_ = val,
    };
}

CVal cval_short(CSigned val)
{
    return (CVal){
        .type = CVAL_SHORT,
        .signed_ = val,
    };
}

CVal cval_int(CSigned val)
{
    return (CVal){
        .type = CVAL_INT,
        .signed_ = val,
    };
}

CVal cval_long(CSigned val)
{
    return (CVal){
        .type = CVAL_LONG,
        .signed_ = val,
    };
}

CVal cval_longlong(CSigned val)
{
    return (CVal){
        .type = CVAL_LONGLONG,
        .signed_ = val,
    };
}

CVal cval_uchar(CUnsigned val)
{
    return (CVal){
        .type = CVAL_UCHAR,
        .signed_ = val,
    };
}

CVal cval_ushort(CUnsigned val)
{
    return (CVal){
        .type = CVAL_USHORT,
        .signed_ = val,
    };
}

CVal cval_uint(CUnsigned val)
{
    return (CVal){
        .type = CVAL_UINT,
        .signed_ = val,
    };
}

CVal cval_ulong(CUnsigned val)
{
    return (CVal){
        .type = CVAL_ULONG,
        .signed_ = val,
    };
}

CVal cval_ulonglong(CUnsigned val)
{
    return (CVal){
        .type = CVAL_ULONGLONG,
        .signed_ = val,
    };
}

CVal cval_float(double val)
{
    return (CVal){
        .type = CVAL_FLOAT,
        .float_ = val,
    };
}

CVal cval_double(double val)
{
    return (CVal){
        .type = CVAL_DOUBLE,
        .float_ = val,
    };
}

CVal cval_str(Str str)
{
    return (CVal){
        .type = CVAL_STRING,
        .str_ = str,
    };
}
