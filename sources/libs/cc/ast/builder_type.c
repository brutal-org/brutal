#include <cc/ast/builder.h>

/* --- CType ---------------------------------------------------------------- */

CType ctype_void(Str name, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_VOID,
        .name = str_dup(name, alloc),
    };
}

CType ctype_auto(Str name, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_AUTO,
        .name = str_dup(name, alloc),
    };
}

CType ctype_bool(Str name, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_BOOL,
        .name = str_dup(name, alloc),
    };
}

CType ctype_ptr(CType subtype, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_PTR,
        .ptr_.subtype = alloc_move(alloc, subtype),
    };
}

CType ctype_array(Str name, CType subtype, int size, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_ARRAY,
        .name = str_dup(name, alloc),
        .array_ = {
            .subtype = alloc_move(alloc, subtype),
            .size = size,
        },
    };
}

CType ctype_signed(Str name, int precision, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_SIGNED,
        .name = str_dup(name, alloc),
        .signed_.precision = precision,
    };
}

CType ctype_unsigned(Str name, int precision, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_UNSIGNED,
        .name = str_dup(name, alloc),
        .unsigned_.precision = precision,
    };
}

CType ctype_float(Str name, int precision, Alloc *alloc)
{
    return (CType){
        .type = CTYPE_FLOAT,
        .name = str_dup(name, alloc),
        .float_.precision = precision,
    };
}

CType ctype_struct(Str name, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_STRUCT,
        .name = str_dup(name, alloc),
    };

    vec_init(&type.struct_.members, alloc);

    return type;
}

CType ctype_union(Str name, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_UNION,
        .name = str_dup(name, alloc),
    };

    vec_init(&type.union_.members, alloc);

    return type;
}

void ctype_member(CType *self, Str name, CType type, Alloc *alloc)
{
    CTypeMember member = {
        .name = str_dup(name, alloc),
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

CType ctype_enum(Str name, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_ENUM,
        .name = str_dup(name, alloc),
    };

    vec_init(&type.enum_.constants, alloc);

    return type;
}

void ctype_constant(CType *self, Str name, CVal val, Alloc *alloc)
{
    CTypeConstant constant = {
        .name = str_dup(name, alloc),
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

void ctype_constant_no_value(CType *self, Str name, Alloc *alloc)
{
    CTypeConstant constant = {
        .name = str_dup(name, alloc),
        .value = {.type = CVAL_INVALID}};

    if (self->type == CTYPE_ENUM)
    {
        vec_push(&self->enum_.constants, constant);
    }
    else
    {
        panic$("Only enums can have constants!");
    }
}

CType ctype_func(CType ret, Str name, Alloc *alloc)
{
    CType type = {
        .type = CTYPE_FUNC,
        .name = str_dup(name, alloc),
    };

    type.func_.ret = alloc_move(alloc, ret);

    vec_init(&type.func_.params, alloc);
    return type;
}

CType ctype_name(Str name, Alloc *alloc)
{
    CType type =
        {
            .type = CTYPE_NAME,
            .name = str_dup(name, alloc),
        };

    return type;
}

CType ctype_attr(CType type, CTypeAttr attr)
{
    type.attr |= attr;
    return type;
}
