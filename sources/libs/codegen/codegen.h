#pragma once
#include <cc/ast/unit.h>

void codegen_from_cunit(CUnit* cunit, IoWriter* object_writer);
