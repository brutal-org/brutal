#pragma once

#include <cc/ast/unit.h>

CUnit cunit(Alloc *alloc);

void cunit_member(CUnit *unit, CUnitEntry entry);

CUnitEntry cunit_decl(CDecl decl);

CUnitEntry cunit_pragma(Str text, Alloc *alloc);

CUnitEntry cunit_pragma_once(Alloc *alloc);

CUnitEntry cunit_define(Str name, CExpr expression, Alloc *alloc);

CUnitEntry cunit_include(bool system, Str path, Alloc *alloc);
