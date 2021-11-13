#pragma once

#include <bid/ast.h>
#include <cc/ast.h>

CUnit bidgen_c_header(BidIface const iface, Alloc *alloc);

CUnit bidgen_c_source(BidIface const iface, Alloc *alloc);
