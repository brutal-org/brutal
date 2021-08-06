#pragma once

#include <brutal/alloc.h>
#include <brutal/io.h>
#include <bs/sexpr.h>

BsExpr bs_parse(Scan *scan, Alloc *alloc);

void bs_dump(BsExpr const *expr, IoWriter *write);

BsExpr bs_eval(BsExpr expr, BsExpr *env, Alloc *alloc);

/* --- Environment ---------------------------------------------------------- */

BsExpr bs_env_def(BsExpr env, BsExpr key, BsExpr value, Alloc *alloc);

BsExpr bs_env_set(BsExpr env, BsExpr key, BsExpr value, Alloc *alloc);

BsExpr bs_env_lookup(BsExpr env, BsExpr key);

BsExpr bs_env_default(Alloc *alloc);
