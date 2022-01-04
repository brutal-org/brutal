#pragma once
#include <brutal/base.h>
#include <brutal/ds/vec.h>
#include <ud/ast/type.h>

typedef struct ast_node UdAstNode;
typedef struct ud_expr UdExpr;

typedef struct
{
    UdExpr *value;
    UdType type;
    bool mutable;
} UdVarDecl;

typedef enum
{
    UD_DECL_NONE,
    UD_DECL_FUNC,
    UD_DECL_VAR,
    UD_DECL_TYPE,
} UdDeclType;

typedef struct
{
    Str name;
    UdType type;
} UdFuncParam;

typedef struct
{
    Vec(UdAstNode) body;
    Vec(UdFuncParam) params;

    UdType return_type;

} UdFuncDecl;

typedef enum
{
    UD_TYPE_RECORD,
    UD_TYPE_TUPLE,
} UdTypeDeclType;

typedef struct
{
    Vec(UdFuncParam) fields;
} UdRecordDecl;

typedef struct
{
    Vec(UdType) fields;
} UdTupleDecl;

typedef struct
{
    UdTypeDeclType type;

    union
    {
        UdRecordDecl record;
        UdTupleDecl tuple;
    };
} UdTypeDecl;

typedef struct
{

    UdDeclType type;

    Str name;

    union
    {
        UdVarDecl var;
        UdFuncDecl func;
        UdTypeDecl type_decl;
    };

} UdDecl;
