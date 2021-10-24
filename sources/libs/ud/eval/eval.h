#pragma once

#include <ud/ast/sexpr.h>

UdExpr ud_eval(UdExpr expr, UdExpr *env, Alloc *alloc);
