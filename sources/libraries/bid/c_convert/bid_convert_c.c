#include <bid/bid.h>
#include <bid/c_convert/bid_convert_c.h>
#include <bid/c_convert/bid_convert_errors.h>
#include <bid/c_convert/bid_convert_header.h>
#include <bid/c_convert/bid_convert_method.h>
#include <bid/c_convert/bid_convert_type.h>
#include <bid/c_convert/bid_convert_typedef.h>
#include <bid/parsing/bid_ast.h>
#include <brutal/log.h>
#include "brutal/text/str.h"

static void bid_write_node(IoWriter *writer, struct bid_ast_node *node, Str current_namespace)
{
    switch (node->type)
    {
    case BID_AST_NODE_TYPE_INTERFACE:
        current_namespace = bid_write_interface(writer, node);
        for (int i = 0; i < node->children.length; i++)
        {
            bid_write_node(writer, node->children.data[i], current_namespace);
        }
        break;
    case BID_AST_NODE_TYPE_METHOD:
        bid_write_method(writer, node, current_namespace);
        break;
    case BID_AST_NODE_TYPE_TYPEDEF:
        bid_write_typedef(writer, node);
        break;
    case BID_AST_NODE_TYPE_ERROR:
        bid_write_errors(writer, node, current_namespace);
        break;

    default:
        break;
    }
}

void convert_bid_to_c(const struct bid *bid, IoWriter *writer)
{
    bid_write_header(bid, writer);

    bid_write_node(writer, bid->root_ast, str_cast(""));
}
