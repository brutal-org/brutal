#pragma once

#include <cc/ast/decl.h>

CDecl cdecl_empty(void);

CDecl cdecl_type(Str name, CType type);

CDecl cdecl_var(Str name, CType type, CExpr expr);

CDecl cdecl_func(Str name, CType type, CStmt body);

CDecl cdecl_attrib(CDecl decl, CDeclAttr attr);
