#pragma once
#include <bid/bid.h>
#include <bid/parsing/bid_ast.h>
#include <brutal/base.h>
#include <brutal/io.h>

void bid_write_errors(IoWriter *writer, struct bid_ast_node *errors, Str namespace);
