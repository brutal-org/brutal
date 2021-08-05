#pragma once

#include <brutal/alloc.h>
#include <brutal/io.h>
#include <bs/sexpr.h>

BsExpr bs_parse(Scan *scan, Alloc *alloc);

void bs_dump(BsExpr const *expr, IoWriter *write);
