
#include <bid/parsing/bid_ast.h>
#include <bid/parsing/bid_error.h>
#include <brutal/base.h>

BidParseResult scan_error(struct bid *idl_in, struct bid_ast_node *errors_node)
{
    // errors {
    // err,
    // err2,
    //};

    skip_comment_and_space(idl_in);

    // errors [{]
    // err,
    // err2,
    //};

    if (scan_curr(&idl_in->scanner) != '{')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_OPENNING_BRACKETS, idl_in));
    }

    // errors {
    // [err,
    // err2,]
    //};

    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    while (scan_curr(&idl_in->scanner) != '}' && !scan_end(&idl_in->scanner))
    {
        skip_comment_and_space(idl_in);
        auto ast = create_ast_node(BID_AST_NODE_TYPE_ERROR_MEMBER);
        vec_push(&errors_node->children, ast);

        // [name],

        Str name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

        if (name.len == 0)
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
        }

        ast->errors.name = name;
        skip_comment_and_space(idl_in);

        // name[,]

        if (scan_curr(&idl_in->scanner) == ',')
        {
            scan_next(&idl_in->scanner);
            skip_comment_and_space(idl_in);
        }

        // name[}]

        else if (scan_curr(&idl_in->scanner) != '}')
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_MEMBERS_BLOCK, idl_in));
        }
    }
    skip_comment_and_space(idl_in);

    if (scan_end(&idl_in->scanner))
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_CLOSING_BRACKETS, idl_in));
    }

    // name[}];

    scan_next(&idl_in->scanner); // '}'

    // name}[;]

    if (scan_curr(&idl_in->scanner) == ';')
    {
        scan_next(&idl_in->scanner); // ';'
        return BID_SUCCESS;
    }
    return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_LINE, idl_in));
}
