#include <cc/builder/decl.h>

CDecl cdecl_empty(void)
{
    return (CDecl){
        .type = CDECL_EMPTY,
    };
}

CDecl cdecl_type(Str name, CType type)
{
    return (CDecl){
        .type = CDECL_TYPE,
        .name = name,
        .type_.type = type,
    };
}

CDecl cdecl_var(Str name, CType type, CExpr expr)
{
    return (CDecl){
        .type = CDECL_VAR,
        .name = name,
        .var_ = {
            .type = type,
            .expr = expr,
        },
    };
}

CDecl cdecl_func(Str name, CType type, CStmt body)
{
    return (CDecl){
        .type = CDECL_FUNC,
        .name = name,
        .func_ = {
            .type = type,
            .body = body,
        },
    };
}

CDecl cdecl_attrib(CDecl decl, CDeclAttr attr)
{
    decl.attr |= attr;
    return decl;
}
