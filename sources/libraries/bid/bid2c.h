#pragma once
#include <bid/ast.h>
#include <bid/bid.h>
#include <brutal/base.h>
#include <brutal/io.h>

void bid2c(const struct bid *bid, IoWriter *writer);
