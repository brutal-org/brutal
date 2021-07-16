#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/bid/bid.h>
#include <brutal/bid/bid_convert_c.h>
#include <brutal/log.h>
#include "brutal/text/str.h"

static void bid_write_header(IoWriter *writer)
{

    print(writer, "#pragma once\n\n");
    print(writer, "#include <brutal/base.h>\n\n");
    print(writer, "// -- generated idl file --\n\n");
}

static void bid_write_interface(IoWriter *writer, struct bid_ast_node *interface)
{
    print(writer, "// interface: {}\n\n", interface->interface.name);
}

static void bid_write_node(IoWriter *writer, struct bid_ast_node *node)
{
    switch (node->type)
    {
    case BID_AST_NODE_TYPE_INTERFACE:
        bid_write_interface(writer, node);
        break;
    default:
        break;
    }

    for (int i = 0; i < node->children.length; i++)
    {
        bid_write_node(writer, &node->children.data[i]);
    }
}

void convert_bid_to_c(const struct bid *bid, IoWriter *writer)
{

    bid_write_header(writer);

    bid_write_node(writer, bid->root_ast);
}
