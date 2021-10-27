#pragma once
#include <cc/ast/unit.h>

typedef void Module;
typedef Result(Error, Module*) CGenResult;

void cgen_llvm_init();
CGenResult cgen_llvm_unit(CUnit cunit);
void cgen_llvm_dump(Module* module);
MaybeError cgen_llvm_compile(Module* module, IoWriter *object_writer);
