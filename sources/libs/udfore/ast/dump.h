#pragma once

#include <json>

#include "ast.h"

Json ast_dump(UdAst *ast, Alloc *alloc);
