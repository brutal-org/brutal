#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/bid/bid.h>
#include <brutal/bid/bid_ast.h>
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

static void bid_write_type(IoWriter *writer, struct bid_ast_node *type)
{
    if (type->children.length != 0)
    {
        print(writer, "{}(", type->ntype.name);
        for (int i = 0; i < type->children.length; i++)
        {
            bid_write_type(writer, type->children.data[i]);
            if (i + 1 < type->children.length)
            {
                print(writer, ", ");
            }
            else
            {
                print(writer, ")");
            }
        }
    }
    else
    {
        print(writer, "{}", type->ntype.name);
    }
}

static void bid_write_typedef(IoWriter *writer, struct bid_ast_node *typedef_node)
{
    if (typedef_node->children.length < 2)
    {
        return;
    }

    struct bid_ast_node *type_node = typedef_node->children.data[1];
    struct bid_ast_node *name_node = typedef_node->children.data[0];

    print(writer, "typedef ");
    bid_write_type(writer, type_node);

    print(writer, " ");
    bid_write_type(writer, name_node);

    print(writer, "; \n\n");
}

void bid_write_argument(IoWriter *writer, struct bid_ast_node *arg)
{
    struct bid_ast_node *target_type = NULL;
    for (int i = 0; i < arg->children.length; i++)
    {
        if (arg->children.data[i]->type == BID_AST_NODE_TYPE)
        {
            target_type = arg->children.data[i];
            break;
        }
    }

    if (target_type == NULL)
    {
        // maybe later add a Any type
        print(writer, "void* {}", arg->argument.name);
        return;
    }
    bid_write_type(writer, target_type);

    print(writer, " {}", arg->argument.name);
}
static void bid_write_method(IoWriter *writer, struct bid_ast_node *method)
{

    struct bid_ast_node *return_value = NULL;
    int arg_count = method->children.length;
    for (int i = 0; i < method->children.length; i++)
    {
        if (method->children.data[i]->type == BID_AST_NODE_TYPE_METHOD_RETURN_TYPE)
        {
            return_value = method->children.data[i];
        }
    }

    if (return_value != NULL)
    {
        bid_write_type(writer, return_value);
        arg_count -= 1; // argument count = ast child - 1
    }
    else
    {
        print(writer, "void");
    }

    print(writer, " {}(", method->method.name);
    for (int i = 0; i < method->children.length; i++)
    {
        if (method->children.data[i]->type == BID_AST_NODE_TYPE_VAR)
        {
            bid_write_argument(writer, method->children.data[i]);
            if (i + 1 < arg_count)
            {
                print(writer, ", ");
            }
        }
    }
    print(writer, ");\n\n");
}
static void bid_write_node(IoWriter *writer, struct bid_ast_node *node)
{
    switch (node->type)
    {
    case BID_AST_NODE_TYPE_INTERFACE:
        bid_write_interface(writer, node);
        for (int i = 0; i < node->children.length; i++)
        {
            bid_write_node(writer, node->children.data[i]);
        }
        break;
    case BID_AST_NODE_TYPE_METHOD:
        bid_write_method(writer, node);
        break;
    case BID_AST_NODE_TYPE_TYPEDEF:
        bid_write_typedef(writer, node);
        break;

    default:
        break;
    }
}

void convert_bid_to_c(const struct bid *bid, IoWriter *writer)
{

    bid_write_header(writer);

    bid_write_node(writer, bid->root_ast);
}
