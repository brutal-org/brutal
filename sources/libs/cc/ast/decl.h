#pragma once

#include <brutal/base.h>
#include <cc/ast/expr.h>
#include <cc/ast/stmt.h>
#include <cc/ast/type.h>

typedef enum
{
    CDECL_NIL,

    CDECL_EMPTY,
    CDECL_TYPE,
    CDECL_VAR,
    CDECL_FUNC,
} CDeclType;

typedef enum
{
    CDECL_AUTO = (1 << 0),
    CDECL_STATIC = (1 << 1),
    CDECL_REGISTER = (1 << 2),
    CDECL_INLINE = (1 << 3),
    CDECL_EXTERN = (1 << 4),
    CDECL_THREAD = (1 << 5),
    CDECL_NO_RETURN = (1 << 6),
} CDeclAttr;

typedef struct
{
    Str name;
    CType type;
} CDeclarator;

typedef struct cdecl
{
    CDeclType type;
    CDeclAttr attr;
    Str name;

    union
    {
        struct
        {
            CType type;
        } type_;

        struct
        {
            CType type;
            CExpr expr;
        } var_;

        struct
        {
            CType type;
            CStmt body;
        } func_;
    };
} CDecl;
