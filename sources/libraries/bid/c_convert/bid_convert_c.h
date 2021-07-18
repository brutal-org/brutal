#pragma once
#include <brutal/base.h>
#include <bid/bid.h>
#include <bid/parsing/bid_ast.h>
#include <brutal/io.h>

void convert_bid_to_c(const struct bid *bid, IoWriter *writer);
