#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io/scan.h>

enum bid_error_type
{
    BID_ERROR_TYPE_INVALID,
    BID_ERROR_TYPE_UNEXPECTED_TOKEN,
};

struct bid_error
{
    enum bid_error_type type;
    int position;
    Str message;
    Str token;
};

#define BID_SUCCESS (OK(BidParseResult, (MonoState){}))

struct bid
{
    struct bid_ast_node *root_ast;
    Scan scan;
};

typedef Result(struct bid_error, struct bid) BidResult;

BidResult bid_init(Str source);

void bid_deinit(struct bid *in);

struct bid_error bid_create_error(enum bid_error_type type, Str error_msg, const struct bid *idl_in);

struct bid_error bid_error(Str expected_token, const struct bid *idl_in);
