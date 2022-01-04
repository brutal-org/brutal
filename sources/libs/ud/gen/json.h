#pragma once
#include <brutal/base.h>
#include <brutal/io.h>
#include <ud/ast.h>

void ud_emit_json(UdAst ast, Emit* emit, Alloc* alloc);
