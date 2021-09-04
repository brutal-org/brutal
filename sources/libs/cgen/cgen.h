#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>

enum DeclarationType
{
    CG_DECL_NONE,
    CG_DECL_VAR,
    CG_DECL_IDENTIFIER,
    CG_DECL_FUNCTION,
};

enum DeclarationSpecifiers
{
    CG_DS_TYPEDEF = 1 << 1,
    CG_DS_EXTERN = 1 << 2,
    CG_DS_STATIC = 1 << 3,
    CG_DS_AUTO = 1 << 4,
    CG_DS_REGISTER = 1 << 5,
    CG_DS_STRUCT = 1 << 6, // in the case of: struct value v;
    CG_DS_UNION = 1 << 7,
    CG_DS_INLINE = 1 << 8,
    CG_DS_CONST = 1 << 9,
    CG_DS_RESTRICT = 1 << 10,
    CG_DS_VOLATILE = 1 << 11,
};

typedef Str CGIdentifier;
typedef struct
{

    CGIdentifier identifier;
} CG_Type;

typedef struct
{
    CG_Type type;

} DeclarationSpecifier;
typedef struct
{
    enum DeclarationType type;
} Declaration;
