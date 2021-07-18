#pragma once
#include <brutal/base.h>
#include <bid/bid.h>
#include <bid/parsing/bid_ast.h>
#include <brutal/io.h>

void bid_write_header(const struct bid *bid, IoWriter *writer);

Str bid_write_interface(IoWriter *writer, struct bid_ast_node *interface);
