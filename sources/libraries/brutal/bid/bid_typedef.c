#include <brutal/bid/bid.h>
#include <brutal/bid/bid_type.h>
#include <brutal/bid/bid_typedef.h>
#include "brutal/io/scan.h"

BidParseResult scan_typedef(struct bid *idl_in, struct bid_ast_node *typedef_node)
{
    // typedef type1 = type2;

    skip_comment_and_space(idl_in);

    // typedef [type1] = type2;

    auto ast = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&typedef_node->children, ast);
    TRY(BidParseResult, scan_type(idl_in, ast));

    skip_comment_and_space(idl_in);
    // typedef type1 [=] type2;
    if (scan_curr(&idl_in->scanner) != '=')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_EQUAL, idl_in));
    }
    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    // typedef type1 = [type2];

    auto ast2 = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&typedef_node->children, ast2);
    TRY(BidParseResult, scan_type(idl_in, ast2));
    skip_comment_and_space(idl_in);
    // typedef type1 = [type2][;]
    if (scan_curr(&idl_in->scanner) != ';')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_LINE, idl_in));
    }
    scan_next(&idl_in->scanner);
    return OK(BidParseResult, (MonoState){});
}
