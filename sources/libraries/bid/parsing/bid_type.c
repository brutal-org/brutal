#include <bid/parsing/bid_type.h>

BidParseResult scan_type(struct bid *idl_in, struct bid_ast_node *type_node)
{
    // [type]

    skip_comment_and_space(idl_in);
    Str name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
    }
    type_node->ntype.name = name;
    skip_comment_and_space(idl_in);

    // type[<]

    if (scan_curr(&idl_in->scanner) == '<')
    {
        scan_next(&idl_in->scanner);

        // type<...>

        while (scan_curr(&idl_in->scanner) != '>' && !scan_end(&idl_in->scanner))
        {

            auto ast = create_ast_node(BID_AST_NODE_TYPE);
            vec_push(&type_node->children, ast);

            // type<[type]>

            scan_type(idl_in, ast);

            // type<type[,]type>

            if (scan_curr(&idl_in->scanner) == ',')
            {

                scan_next(&idl_in->scanner);
            }

            // type<type[>]
            else if (scan_curr(&idl_in->scanner) != '>')
            {

                return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_TYPE, idl_in));
            }
        }

        if (scan_end(&idl_in->scanner))
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_GREATER_THAN, idl_in));
        }

        scan_next(&idl_in->scanner); // skip '>'
    }

    return BID_SUCCESS;
}
