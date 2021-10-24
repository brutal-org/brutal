#pragma once

#include <ud/ast/sexpr.h>

UdExpr ud_env_def(UdExpr env, UdExpr key, UdExpr value, Alloc *alloc);

UdExpr ud_env_set(UdExpr env, UdExpr key, UdExpr value, Alloc *alloc);

UdExpr ud_env_lookup(UdExpr env, UdExpr key);

UdExpr ud_env_default(Alloc *alloc);
