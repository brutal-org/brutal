#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io/scan.h>

#define BID_INTERFACE_TOKEN (str_cast("interface"))

enum bid_error_type
{
    BID_ERROR_TYPE_INVALID,
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
};

enum bid_ast_node_type
{
    BID_AST_NODE_TYPE_UNKNOWN,
    BID_AST_NODE_TYPE_INTERFACE,
};

struct bid_ast_node
{
    enum bid_ast_node_type type;
    Vec(struct bid_ast_node) children;

    union
    {
        struct ast_node_interface
        {
            Str name;
        } interface;

    };
};

struct bid
{
    Str in_data;

    struct bid_ast_node root_ast;
    struct scan scanner;
    int _current_scanned_token_cursor;
};
typedef Result(struct bid_error, MonoState) BidParseResult;
typedef Result(struct bid_error, struct bid) BidResult;

BidResult init_bid(Str idl_in);

Str bid_to_c(struct bid from);

enum bid_token_type bid_token_from_char(char from);

enum bid_keywords bid_keyword_from_string(Str from);
