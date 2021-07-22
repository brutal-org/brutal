#pragma once
#include <bid/ast.h>
#include <brutal/base.h>

typedef Result(struct bid_error, MonoState) BidParseResult;

BidParseResult bid_parse(struct bid *idl_in);
