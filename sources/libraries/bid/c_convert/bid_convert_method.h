#pragma once
#include <brutal/base.h>
#include <bid/bid.h>
#include <bid/parsing/bid_ast.h>
#include <brutal/io.h>

void bid_write_method(IoWriter *writer, struct bid_ast_node *method, Str current_namespace);
