#pragma once
#include <bid/bid.h>
#include <bid/parsing/bid_ast.h>
#include <brutal/base.h>
#include <brutal/io.h>

void bid_convert_message_type(IoWriter *writer, struct bid_ast_node *interface, Str namespace);
