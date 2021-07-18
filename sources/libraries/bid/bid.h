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

typedef Result(struct bid_error, MonoState) BidParseResult;
typedef Result(struct bid_error, struct bid) BidResult;

BidResult init_bid(Str idl_in);

void destroy_bid(struct bid *in);

enum bid_token_type bid_token_from_char(char from);

enum bid_keywords bid_keyword_from_string(Str from);

struct bid_error bid_create_error(enum bid_error_type type, Str error_msg, const struct bid *idl_in);

struct bid_error bid_create_unexpected_token_error(Str expected_token, const struct bid *idl_in);

int bid_is_keyword(int chr);

void skip_comment_and_space(struct bid *idl_in);
