#include <ansi/ctypes.h>
#include <bid/bid.h>
#include <bid/parser.h>
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/log.h>

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

static BidParseResult scan_bid(struct bid *idl_in)
{
    TRY(BidParseResult, scan_interface_definition(idl_in));

    return BID_SUCCESS;
}

BidResult bid_init(Str idl_in)
{
    struct bid mbid = TRY(BidResult, create_bid(idl_in));

    TRY(BidResult, scan_bid(&mbid));

    return OK(BidResult, mbid);
}

void bid_deinit(struct bid *in)
{
    destroy_ast_node_recursive(in->root_ast);
}
