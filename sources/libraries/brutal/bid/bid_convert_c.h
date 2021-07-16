#pragma once
#include <brutal/bid/bid.h>
#include <brutal/base.h>
#include <brutal/io.h>

void convert_bid_to_c(const struct bid* bid, IoWriter* writer);
