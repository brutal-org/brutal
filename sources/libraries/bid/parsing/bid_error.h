#pragma once
#include <bid/parsing/bid_ast.h>
#include <brutal/base.h>

BidParseResult scan_error(struct bid *idl_in, struct bid_ast_node *method_node);
