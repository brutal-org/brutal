#pragma once

#include <brutal/parse.h>
#include <ud/ast/sexpr.h>

UdExpr ud_parse(Scan *scan, Alloc *alloc);
