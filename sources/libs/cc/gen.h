#pragma once

#include <brutal/io/emit.h>
#include <cc/ast/translation_unit.h>

void c_generate(Emit *target, CTransUnit unit);
