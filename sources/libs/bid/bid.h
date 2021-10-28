#pragma once

#include <bid/ast.h>
#include <brutal/io.h>
#include <brutal/parse.h>

BidInterface bid_parse(Scan *scan, Alloc *alloc);

void bid2c(BidInterface const *interface, Emit *emit, Alloc *alloc);
