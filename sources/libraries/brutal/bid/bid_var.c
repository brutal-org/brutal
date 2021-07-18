#include <brutal/bid/bid_type.h>
#include <brutal/bid/bid_var.h>

BidParseResult scan_var(struct bid *idl_in, struct bid_ast_node *arg_node)
{
    // [name] : type

    skip_comment_and_space(idl_in);
    Str name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
    }
    arg_node->argument.name = name;

    skip_comment_and_space(idl_in);

    // name [:] type

    if (scan_curr(&idl_in->scanner) != ':')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_DOUBLE_DOT, idl_in));
    }
    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    // name : [type]

    auto ast = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&arg_node->children, ast);

    TRY(BidParseResult, scan_type(idl_in, ast));

    return BID_SUCCESS;
}
