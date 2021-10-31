#pragma once 

#include <brutal/io.h>
#include <cc/ast/unit.h>

void cgen_c_type(Emit *emit, CType type);

void cgen_c_type_start(Emit *emit, CType type);

void cgen_c_type_end(Emit *emit, CType type);
