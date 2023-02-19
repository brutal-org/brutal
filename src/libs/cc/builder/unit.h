#pragma once

#include <cc/ast/unit.h>

CUnit cunit(Alloc *alloc);

void cunit_decl(CUnit *self, CDecl decl);

void cunit_pragma(CUnit *self, Str text);

void cunit_pragma_once(CUnit *self);

void cunit_define(CUnit *self, Str name, CExpr expression, Alloc *alloc);

void cunit_include(CUnit *self, bool system, Str path);
