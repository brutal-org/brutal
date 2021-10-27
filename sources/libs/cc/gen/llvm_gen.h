#pragma once
#include <cc/ast/unit.h>

void codegen_init();
MaybeError codegen_from_unit(CUnit cunit, IoWriter *object_writer);
