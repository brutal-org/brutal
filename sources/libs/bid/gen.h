#pragma once

#include <bid/ast.h>
#include <bid/gen/json.h>
#include <cc/ast.h>

CUnit bidgen_c(BidIface const *interface, Alloc *alloc);
