#include <brutal/bid/bid_method.h>
#include <brutal/bid/bid_type.h>
#include <brutal/bid/bid_var.h>
#include "brutal/base/types.h"

BidParseResult scan_method(struct bid *idl_in, struct bid_ast_node *method_node)
{
    // method [name](..)->...;
    skip_comment_and_space(idl_in);
    Str name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
    }
    method_node->method.name = name;
    // method name[(]...)->...;
    skip_comment_and_space(idl_in);
    if (scan_curr(&idl_in->scanner) != '(')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_OPENNING_PARENTHESIS, idl_in));
    }

    // method name([...])->...;
    scan_next(&idl_in->scanner);
    while (scan_curr(&idl_in->scanner) != ')' && !scan_end(&idl_in->scanner))
    {
        // method name(arg)->...;
        skip_comment_and_space(idl_in);
        auto ast = create_ast_node(BID_AST_NODE_TYPE_VAR);
        vec_push(&method_node->children, ast);

        TRY(BidParseResult, scan_var(idl_in, ast));

        skip_comment_and_space(idl_in);

        // method name(arg[,] )->...;
        if (scan_curr(&idl_in->scanner) == ',')
        {

            scan_next(&idl_in->scanner);
            skip_comment_and_space(idl_in);
        }
        // method name(arg[)]->...;
        else if (scan_curr(&idl_in->scanner) != ')')
        {

            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_ARGUMENT, idl_in));
        }
    }
    if (scan_end(&idl_in->scanner))
    {

        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_CLOSING_PARENTHESIS, idl_in));
    }
    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    // method name()[;]
    if (scan_curr(&idl_in->scanner) == ';')
    {
        scan_next(&idl_in->scanner); // ;
        return BID_SUCCESS;
    }

    // method name() [->] type;

    if (scan_curr(&idl_in->scanner) != '-' || scan_peek(&idl_in->scanner, 1) != '>')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ARROW, idl_in));
    }
    scan_next(&idl_in->scanner); // skip [-]>
    scan_next(&idl_in->scanner); // skip -[>]
    skip_comment_and_space(idl_in);

    auto ast = create_ast_node(BID_AST_NODE_TYPE_METHOD_RETURN_TYPE);
    vec_push(&method_node->children, ast);

    TRY(BidParseResult, scan_type(idl_in, ast));

    skip_comment_and_space(idl_in);

    // method name() -> type[;]
    if (scan_curr(&idl_in->scanner) != ';')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_LINE, idl_in));
    }
    scan_next(&idl_in->scanner); // ;

    return BID_SUCCESS;
}
