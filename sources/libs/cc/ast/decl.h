#pragma once

#include <brutal/base.h>
#include <cc/ast/expr.h>
#include <cc/ast/stmt.h>
#include <cc/ast/type.h>

typedef enum
{
    CDECL_INVALID,

    CDECL_TYPE,
    CDECL_VAR,
    CDECL_FUNC,
} CDeclType;

#define CDECL_AUTO (1 << 0)
#define CDECL_STATIC (1 << 1)
#define CDECL_REGISTER (1 << 2)
#define CDECL_INLINE (1 << 3)
#define CDECL_EXTERN (1 << 4)
#define CDECL_THREAD (1 << 5)

typedef unsigned int CDeclAttr;

typedef struct cdecl CDecl;

struct cdecl
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
};
