#pragma once
#include <bid/ast.h>
#include <brutal/base.h>

typedef Result(struct bid_error, MonoState) BidParseResult;

BidParseResult scan_interface_definition(struct bid *idl_in);
