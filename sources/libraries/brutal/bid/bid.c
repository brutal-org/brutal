#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/bid/bid.h>
#include <brutal/log.h>
#include <ctype.h>
#include "brutal/alloc/base.h"
#include "brutal/base/attributes.h"
#include "brutal/base/types.h"
#include "brutal/io/scan.h"
#include "brutal/text/str.h"

static BidResult create_bid(Str data_in)
{
    struct bid result =
        {
            .in_data = data_in,
            ._current_scanned_token_cursor = 0,
        };

    scan_init(&result.scanner, data_in);

    return OK(BidResult, result);
}

static bool skip_comment(struct bid *idl_in)
{

    char current = scan_curr(&idl_in->scanner);
    if (current != '/' || scan_peek(&idl_in->scanner, 1) != '*')
    {
        return false;
    }

    while (!scan_end(&idl_in->scanner))
    {
        char current = scan_curr(&idl_in->scanner);
        if (current == '*' && scan_peek(&idl_in->scanner, 1) == '/')
        {
            scan_next(&idl_in->scanner); // skip the '*'
            scan_next(&idl_in->scanner); // skip the '/'
            return true;
        }
        scan_next(&idl_in->scanner);
    }

    if (scan_end(&idl_in->scanner))
    {
        return false;
    }

    return false; // clang warns fix (unreachable code)
}

static void skip_comment_and_space(struct bid *idl_in)
{
    // loop it until we don't hit a comment or a space

    while ((skip_space(&idl_in->scanner).len != 0 ||
            skip_comment(idl_in)))
    {
        if (scan_end(&idl_in->scanner))
        {
            return;
        }
    };
}

static struct bid_ast_node *create_ast_node(enum bid_ast_node_type type)
{
    struct bid_ast_node *node = alloc_malloc(alloc_global(), sizeof(struct bid_ast_node));
    vec_init(&node->children, alloc_global());
    node->type = type;
    return node;
}

static void destroy_ast_node_recursive(struct bid_ast_node *from)
{

    if (from != NULL)
    {
        for (int i = 0; i < from->children.length; i++)
        {
            destroy_ast_node_recursive(&from->children.data[i]);
        }
        vec_deinit(&from->children);
        alloc_free(alloc_global(), from);
    }
}

void print_ast_node_recursive(struct bid_ast_node *from)
{

    if (from != NULL)
    {
        for (int i = 0; i < from->children.length; i++)
        {
            print_ast_node_recursive(&from->children.data[i]);
        }

        log("ast type: {}", from->type);
    }
}

int is_keyword(int chr)
{
    if (isalnum(chr))
    {
        return true;
    }
    return (chr == '_');
}

static struct bid_error bid_create_error(enum bid_error_type type, Str error_msg, const struct bid *idl_in)
{
    return (struct bid_error){
        .message = error_msg,
        .pos = (struct bid_buffer_position){idl_in->scanner.head},
        .type = type};
}

static struct bid_error bid_create_unexpected_token_error(Str expected_token, const struct bid *idl_in)
{
    struct bid_error err_res = bid_create_error(BID_ERROR_TYPE_UNEXPECTED_TOKEN, str_cast("unexpected token"), idl_in);
    err_res.specific_information.expected_token.name = expected_token;
    return err_res;
}

static BidParseResult scan_interface_block(struct bid *idl_in, struct bid_ast_node *interface_node)
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
        // todo: keyword detection here
        scan_next(&idl_in->scanner);
    }

    // interface name  { [}]
    if (scan_curr(&idl_in->scanner) != '}')
    {

        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_CLOSING_BRACKETS, idl_in));
    }
    skip_comment_and_space(idl_in);

    return OK(BidParseResult, (MonoState){});
}

static BidParseResult scan_interface_definition(struct bid *idl_in)
{

    skip_comment_and_space(idl_in);

    Str result = scan_skip_until((&idl_in->scanner), is_keyword);

    // [interface] name { }
    if (str_eq(result, str_cast("interface")))
    {
        auto ast = create_ast_node(BID_AST_NODE_TYPE_INTERFACE);
        idl_in->root_ast = ast; // for the moment support 1 interface per file

        skip_comment_and_space(idl_in);

        // interface [name] { }
        Str interface_name = scan_skip_until((&idl_in->scanner), is_keyword);

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

static BidParseResult scan_bid(struct bid *idl_in)
{
    TRY(BidParseResult, scan_interface_definition(idl_in));

    return OK(BidParseResult, (MonoState){});
}

BidResult init_bid(Str idl_in)
{
    struct bid mbid = TRY(BidResult, create_bid(idl_in));

    TRY(BidResult, scan_bid(&mbid));

    return OK(BidResult, mbid);
}

void destroy_bid(struct bid *in)
{
    destroy_ast_node_recursive(in->root_ast);
}
