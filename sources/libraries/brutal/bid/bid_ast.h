#pragma once
#include <brutal/bid/bid.h>
#include <brutal/ds/vec.h>

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
#define BID_EQUAL (str_cast("="))

enum bid_ast_node_type
{
    BID_AST_NODE_TYPE_UNKNOWN,
    BID_AST_NODE_TYPE_INTERFACE,
    BID_AST_NODE_TYPE_METHOD,
    BID_AST_NODE_TYPE_TYPEDEF,
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

struct bid_ast_node *create_ast_node(enum bid_ast_node_type type);

void destroy_ast_node_recursive(struct bid_ast_node *from);
