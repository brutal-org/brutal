#pragma once
#include <brutal/base.h>
#include <brutal/bid/bid_ast.h>

// warn: don't check for ';' as we can use it for argument parsing:
// foo(x : int[,] v : long);
// and for struct declaration:
// struct my_structure
// {
//      x : int[;]
//      y : long[;]
// };

BidParseResult scan_var(struct bid *idl_in, struct bid_ast_node *method_node);
