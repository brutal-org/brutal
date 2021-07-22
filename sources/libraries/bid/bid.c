#include <ansi/ctypes.h>
#include <bid/bid.h>
#include <bid/parser.h>
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/log.h>

struct bid_error bid_error(Str expected_token, const struct bid *idl_in)
{
    return (struct bid_error){
        .type = BID_ERROR_TYPE_UNEXPECTED_TOKEN,
        .position = idl_in->scan.head,
        .message = str_cast("unexpected token"),
        .token = expected_token,
    };
}

BidResult bid_init(Str source)
{
    struct bid self = {};

    scan_init(&self.scan, source);

    TRY(BidResult, bid_parse(&self));
    return OK(BidResult, self);
}

void bid_deinit(struct bid *in)
{
    destroy_ast_node_recursive(in->root_ast);
}
