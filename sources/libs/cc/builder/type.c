#include <brutal-debug>
#include <cc/builder/type.h>

const char *ctype_type_to_str[CTYPE_COUNT] = {
    [CTYPE_INVALID] = "<invalid>",
    [CTYPE_TAIL] = "<tail>",

    [CTYPE_VOID] = "void",
    [CTYPE_AUTO] = "auto",
    [CTYPE_BOOL] = "bool",
    [CTYPE_PTR] = "",
    [CTYPE_ARRAY] = "",

    [CTYPE_CHAR] = "char",
    [CTYPE_SHORT] = "short",
    [CTYPE_INT] = "int",
    [CTYPE_LONG] = "long",
    [CTYPE_LONGLONG] = "long long",
    [CTYPE_UCHAR] = "unsigned char",
    [CTYPE_USHORT] = "unsigned short",
    [CTYPE_UINT] = "unsigned int",
    [CTYPE_ULONG] = "unsigned long",
    [CTYPE_ULONGLONG] = "unsigned long long",
    [CTYPE_FLOAT] = "float",
    [CTYPE_DOUBLE] = "double",

    [CTYPE_STRUCT] = "struct",
    [CTYPE_UNION] = "union",
    [CTYPE_ENUM] = "enum",
};

Str ctype_to_str(CType type)
{
    if (ctype_type_to_str[type.type])
    {
        return str$(ctype_type_to_str[type.type]);
    }
    else
    {
        return str$("<error>");
    }
}

CType ctype_to_unsigned(CType type)
{
    assert_truth(type.type >= CTYPE_CHAR && type.type <= CTYPE_ULONGLONG);
    type.type = type.type + CTYPE_UCHAR - CTYPE_CHAR;
    return type;
}

/* --- Builder -------------------------------------------------------------- */

CType ctype_tail(void)
{
    return (CType){
        .type = CTYPE_TAIL,
    };
}

CType ctype_error(void)
{
    return (CType){
        .type = CTYPE_INVALID,
    };
}

CType ctype_void(void)
{
    return (CType){
        .type = CTYPE_VOID,
    };
}

CType ctype_auto(void)
{
    return (CType){
        .type = CTYPE_AUTO,
    };
}

CType ctype_bool(void)
{
    return (CType){
        .type = CTYPE_BOOL,
    };
}

CType ctype_ptr(CType subtype, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_PTR,
        .ptr_.subtype = alloc_move(alloc, subtype),
    };
}

CType ctype_parent(CType subtype, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_PARENT,
        .parent_.subtype = alloc_move(alloc, subtype),
    };
}

CType ctype_array(CType subtype, int size, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_ARRAY,
        .array_ = {
            .subtype = alloc_move(alloc, subtype),
            .size = size,
        },
    };
}

CType ctype_char(void)
{
    return (CType){
        .type = CTYPE_CHAR,
    };
}

CType ctype_short(void)
{
    return (CType){
        .type = CTYPE_SHORT,
    };
}

CType ctype_int(void)
{
    return (CType){
        .type = CTYPE_INT,
    };
}

CType ctype_long(void)
{
    return (CType){
        .type = CTYPE_LONG,
    };
}

CType ctype_longlong(void)
{
    return (CType){
        .type = CTYPE_LONGLONG,
    };
}

CType ctype_uchar(void)
{
    return (CType){
        .type = CTYPE_UCHAR,
    };
}

CType ctype_ushort(void)
{
    return (CType){
        .type = CTYPE_USHORT,
    };
}

CType ctype_uint(void)
{
    return (CType){
        .type = CTYPE_UINT,
    };
}

CType ctype_ulong(void)
{
    return (CType){
        .type = CTYPE_ULONG,
    };
}

CType ctype_ulonglong(void)
{
    return (CType){
        .type = CTYPE_ULONGLONG,
    };
}

CType ctype_float(void)
{
    return (CType){
        .type = CTYPE_FLOAT,
    };
}

CType ctype_double(void)
{
    return (CType){
        .type = CTYPE_DOUBLE,
    };
}

CType ctype_struct(Alloc *alloc)
{
    CType type = {
        .type = CTYPE_STRUCT,
    };

    vec_init(&type.struct_.members, alloc);

    return type;
}

CType ctype_union(Alloc *alloc)
{
    CType type = {
        .type = CTYPE_UNION,
    };

    vec_init(&type.union_.members, alloc);

    return type;
}

void ctype_member(CType *self, Str name, CType type)
{
    CTypeMember member = {
        .name = name,
        .type = type,
    };

    if (self->type == CTYPE_STRUCT || self->type == CTYPE_UNION)
    {
        vec_push(&self->struct_.members, member);
    }
    else if (self->type == CTYPE_FUNC)
    {
        vec_push(&self->func_.params, member);
    }
    else
    {
        panic$("Only structs, unions or funcs can have members!");
    }
}

CType ctype_enum(Alloc *alloc)
{
    CType type = {
        .type = CTYPE_ENUM,
    };

    vec_init(&type.enum_.constants, alloc);

    return type;
}

void ctype_constant(CType *self, Str name, CVal val)
{
    CTypeConst constant = {
        .name = name,
        .value = val,
    };

    if (self->type == CTYPE_ENUM)
    {
        vec_push(&self->enum_.constants, constant);
    }
    else
    {
        panic$("Only enums can have constants!");
    }
}

CType ctype_func(CType ret, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_FUNC,
    };

    type.func_.ret = alloc_move(alloc, ret);

    vec_init(&type.func_.params, alloc);
    return type;
}

CType ctype_ident(Str name)
{
    CType type = {
        .type = CTYPE_NAME,
        .name = name,
    };

    return type;
}

CType ctype_attr(CType type, CTypeAttr attr)
{
    type.attr = (CTypeAttr)(type.attr | attr);
    return type;
}

CType ctype_named(CType type, Str name)
{
    type.name = name;
    return type;
}

void ctype_append(CType *type, CType tail)
{
    switch (type->type)
    {
    case CTYPE_TAIL:
        *type = tail;
        break;

    case CTYPE_PTR:
        ctype_append(type->ptr_.subtype, tail);
        break;

    case CTYPE_PARENT:
        ctype_append(type->parent_.subtype, tail);
        break;

    case CTYPE_FUNC:
        ctype_append(type->func_.ret, tail);
        break;

    case CTYPE_ARRAY:
        ctype_append(type->array_.subtype, tail);
        break;

    default:
        panic$("ctype_append on type {}", ctype_to_str(*type));
    }
}
