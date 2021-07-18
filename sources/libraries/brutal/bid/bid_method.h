#pragma once
#include <brutal/base.h>
#include <brutal/bid/bid_ast.h>

BidParseResult scan_method(struct bid *idl_in, struct bid_ast_node *method_node);
