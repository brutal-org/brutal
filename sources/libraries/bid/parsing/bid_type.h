#pragma once
#include <brutal/base.h>
#include <bid/parsing/bid_ast.h>

BidParseResult scan_type(struct bid *idl_in, struct bid_ast_node *type_node);
