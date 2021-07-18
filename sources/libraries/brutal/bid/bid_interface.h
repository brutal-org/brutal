#pragma once
#include <brutal/base.h>
#include <brutal/bid/bid_ast.h>

BidParseResult scan_interface(struct bid *idl_in, struct bid_ast_node *type_node);

BidParseResult scan_interface_definition(struct bid *idl_in);
