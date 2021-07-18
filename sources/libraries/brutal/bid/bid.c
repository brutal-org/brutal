#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/bid/bid.h>
#include <brutal/bid/bid_interface.h>
#include <brutal/log.h>
#include <ctype.h>
#include "brutal/alloc/base.h"
#include "brutal/base/attributes.h"
#include "brutal/base/types.h"
#include "brutal/ds/vec.h"
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

struct bid_error bid_create_error(enum bid_error_type type, Str error_msg, const struct bid *idl_in)
{
    return (struct bid_error){
        .message = error_msg,
        .pos = (struct bid_buffer_position){idl_in->scanner.head},
        .type = type};
}

struct bid_error bid_create_unexpected_token_error(Str expected_token, const struct bid *idl_in)
{
    struct bid_error err_res = bid_create_error(BID_ERROR_TYPE_UNEXPECTED_TOKEN, str_cast("unexpected token"), idl_in);
    err_res.specific_information.expected_token.name = expected_token;
    return err_res;
}

int bid_is_keyword(int chr)
{
    if (isalnum(chr))
    {
        return true;
    }
    return (chr == '_');
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

void skip_comment_and_space(struct bid *idl_in)
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
