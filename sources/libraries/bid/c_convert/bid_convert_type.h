#pragma once
#include <brutal/base.h>
#include <bid/bid.h>
#include <bid/parsing/bid_ast.h>
#include <brutal/io.h>

void bid_write_type(IoWriter *writer, struct bid_ast_node *type);
