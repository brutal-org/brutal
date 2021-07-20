#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io/scan.h>

enum bid_error_type
{
    BID_ERROR_TYPE_INVALID,
    BID_ERROR_TYPE_UNEXPECTED_TOKEN,
};

struct bid_buffer_position
{
    int offset;
};

struct bid_error
{
    enum bid_error_type type;
    struct bid_buffer_position pos;
    Str message;

    union
    {
        struct
        {
            Str name;
        } expected_token;
    } specific_information;
};

#define BID_SUCCESS (OK(BidParseResult, (MonoState){}))

struct bid
{
    Str in_data;

    struct bid_ast_node *root_ast;
    struct scan scanner;
    int _current_scanned_token_cursor;
};

typedef Result(struct bid_error, struct bid) BidResult;

BidResult bid_init(Str idl_in);

void bid_deinit(struct bid *in);

struct bid_error bid_create_error(enum bid_error_type type, Str error_msg, const struct bid *idl_in);

struct bid_error bid_create_unexpected_token_error(Str expected_token, const struct bid *idl_in);
