#pragma once

#include <cc/ast/decl.h>

CDecl cdecl_type(Str name, CType type, Alloc *alloc);

CDecl cdecl_var(Str name, CType type, CExpr expr, Alloc *alloc);

CDecl cdecl_func(Str name, CType type, CStmt body, Alloc *alloc);

CDecl cdecl_attrib(CDecl decl, CDeclAttr attr);
