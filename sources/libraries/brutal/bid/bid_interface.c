#include <brutal/bid/bid.h>
#include <brutal/bid/bid_interface.h>
#include <brutal/bid/bid_method.h>
#include <brutal/bid/bid_typedef.h>

BidParseResult scan_interface_block(struct bid *idl_in, struct bid_ast_node *interface_node)
{
    (void)interface_node;

    // interface name [{]}
    if (scan_curr(&idl_in->scanner) != '{')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_OPENNING_BRACKETS, idl_in));
    }
    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    // interface name  { [...] }
    while (scan_curr(&idl_in->scanner) != '}' && !scan_end(&idl_in->scanner))
    {

        skip_comment_and_space(idl_in);

        Str result = scan_skip_until((&idl_in->scanner), bid_is_keyword);

        if (result.len == 0 && scan_curr(&idl_in->scanner) == '}')
        {
            break;
        }
        else if (result.len == 0 && scan_curr(&idl_in->scanner) != '}')
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
        }
        else if (str_eq(result, str_cast("method")))
        {

            auto ast = create_ast_node(BID_AST_NODE_TYPE_METHOD);
            vec_push(&interface_node->children, ast);

            TRY(BidParseResult, scan_method(idl_in, ast));
        }
        else if (str_eq(result, str_cast("typedef")))
        {

            auto ast = create_ast_node(BID_AST_NODE_TYPE_TYPEDEF);
            vec_push(&interface_node->children, ast);

            TRY(BidParseResult, scan_typedef(idl_in, ast));
        }
        else
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(str_cast("method/struct/enum/typedef"), idl_in));
        }
    }

    // interface name  { [}]
    if (scan_curr(&idl_in->scanner) != '}')
    {

        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_CLOSING_BRACKETS, idl_in));
    }
    skip_comment_and_space(idl_in);

    return OK(BidParseResult, (MonoState){});
}

BidParseResult scan_interface_definition(struct bid *idl_in)
{

    skip_comment_and_space(idl_in);

    Str result = scan_skip_until((&idl_in->scanner), bid_is_keyword);

    // [interface] name { }
    if (str_eq(result, str_cast("interface")))
    {
        auto ast = create_ast_node(BID_AST_NODE_TYPE_INTERFACE);
        idl_in->root_ast = ast; // for the moment support 1 interface per file

        skip_comment_and_space(idl_in);

        // interface [name] { }
        Str interface_name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

        if (interface_name.len == 0)
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
        }

        ast->interface.name = interface_name;

        skip_comment_and_space(idl_in);
        TRY(BidParseResult, scan_interface_block(idl_in, ast));
        return OK(BidParseResult, (MonoState){});
    }
    else
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_INTERFACE_STR, idl_in));
    }

    return OK(BidParseResult, (MonoState){});
}
