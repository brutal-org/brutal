#pragma once

#include <brutal/alloc.h>
#include <brutal/io.h>
#include <udfore/sexpr.h>

UdExpr ud_parse(Scan *scan, Alloc *alloc);

void ud_dump(UdExpr const *expr, IoWriter *write);

UdExpr ud_eval(UdExpr expr, UdExpr *env, Alloc *alloc);

/* --- Environment ---------------------------------------------------------- */

UdExpr ud_env_def(UdExpr env, UdExpr key, UdExpr value, Alloc *alloc);

UdExpr ud_env_set(UdExpr env, UdExpr key, UdExpr value, Alloc *alloc);

UdExpr ud_env_lookup(UdExpr env, UdExpr key);

UdExpr ud_env_default(Alloc *alloc);
