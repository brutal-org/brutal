#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io/scan.h>

#define BID_INTERFACE_STR (str_cast("interface"))
#define BID_ALNUM_STR (str_cast("[a-Az-Z_] string"))
#define BID_CLOSING_BRACKETS (str_cast("}"))
#define BID_OPENNING_BRACKETS (str_cast("{"))
#define BID_CLOSING_PARENTHESIS (str_cast(")"))
#define BID_OPENNING_PARENTHESIS (str_cast("("))
#define BID_END_ARGUMENT (str_cast(") ,"))
#define BID_END_TYPE (str_cast("> ,"))
#define BID_DOUBLE_DOT (str_cast(":"))
#define BID_GREATER_THAN (str_cast(">"))
#define BID_ARROW (str_cast("->"))
#define BID_END_LINE (str_cast(";"))

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

enum bid_ast_node_type
{
    BID_AST_NODE_TYPE_UNKNOWN,
    BID_AST_NODE_TYPE_INTERFACE,
    BID_AST_NODE_TYPE_METHOD,
    BID_AST_NODE_TYPE_METHOD_RETURN_TYPE,   // type
    BID_AST_NODE_TYPE_METHOD_ARGUMENT_TYPE, // argument
    BID_AST_NODE_TYPE
};

struct bid_ast_node
{
    enum bid_ast_node_type type;
    Vec(struct bid_ast_node *) children;

    union
    {
        struct ast_node_interface
        {
            Str name;
        } interface;

        struct ast_node_method
        {
            Str name;
        } method;

        // a type may contain other:
        // if we have Result<MaybeError<int>, Error>
        // we will get a tree that may look like this:
        // Result:
        //      MaybeError:
        //          int
        //      Error

        struct ast_node_type
        {
            Str name;
        } ntype;

        // an argument can have a type
        struct ast_node_argument // argument -> type
        {
            Str name;
        } argument;
    };
};

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

void print_ast_node_recursive(struct bid_ast_node *from);
