
#include <bid/bid2c.h>
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/log.h>

// ------------- header -------------

void bid_write_header(const struct bid *bid, IoWriter *writer)
{
    (void)bid;
    print(writer, "#pragma once\n\n");
    print(writer, "#include <brutal/base.h>\n\n");
    print(writer, "// -- generated idl file --\n\n");
}

// ------------- interface -------------

Str bid_write_interface(IoWriter *writer, struct bid_ast_node *interface)
{
    print(writer, "// interface: {}\n\n", interface->interface.name);
    return interface->interface.name;
}

// ------------- message type -------------

void bid_convert_message_type(IoWriter *writer, struct bid_ast_node *interface, Str current_namespace)
{
    print(writer, "enum {}_messages_type\n\\{\n", current_namespace);

    for (int i = 0; i < interface->children.length; i++)
    {
        if (interface->children.data[i]->type == BID_AST_NODE_TYPE_METHOD)
        {
            struct bid_ast_node *method_node = interface->children.data[i];
            Str name = method_node->method.name;

            print(writer, "\t{}_msg_{},\n", current_namespace, name);
        }
    }
    print(writer, "};\n\n");
}

// ------------- types --------------

void bid_write_type(IoWriter *writer, struct bid_ast_node *type)
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

// ------------- typedefs -----------

void bid_write_typedef(IoWriter *writer, struct bid_ast_node *typedef_node)
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

    print(writer, ";\n\n");
}

// ------------- methods ------------

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

static void bid_write_method_send_structure(IoWriter *writer, struct bid_ast_node *method, Str current_namespace)
{
    print(writer, "struct {}_{}_send\n\\{\n", current_namespace, method->method.name);

    for (int i = 0; i < method->children.length; i++)
    {
        if (method->children.data[i]->type == BID_AST_NODE_TYPE_VAR)
        {
            print(writer, "\t");

            bid_write_argument(writer, method->children.data[i]);

            print(writer, ";\n");
        }
    }

    print(writer, "};\n\n");
}

void bid_write_method(IoWriter *writer, struct bid_ast_node *method, Str current_namespace)
{

    print(writer, "// - {}.{}\n\n", current_namespace, method->method.name);

    //  bid_write_method_received_structure(writer, method);

    bid_write_method_send_structure(writer, method, current_namespace);

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

    print(writer, " {}_{}(", current_namespace, method->method.name);

    print(writer, "BrTask target, struct {}_{}_send* send", current_namespace, method->method.name);

    print(writer, ");\n\n");
}

// ------------- errors -------------

void bid_write_errors(IoWriter *writer, struct bid_ast_node *errors, Str namespace)
{

    print(writer, "enum {}_errors_type\n\\{\n", namespace);
    print(writer, "\t{}_{},\n", namespace, "no_error");

    for (int i = 0; i < errors->children.length; i++)
    {
        if (errors->children.data[i]->type == BID_AST_NODE_TYPE_ERROR_MEMBER)
        {
            print(writer, "\t{}_{},\n", namespace, errors->children.data[i]->errors.name);
        }
    }

    print(writer, "};\n\n");

    print(writer, "struct {}_errors\n\\{\n", namespace);
    print(writer, "\tenum {}_errors_type type;\n", namespace);
    print(writer, "};\n\n");
}

// ------------- C converter -------------

static void bid_write_node(IoWriter *writer, struct bid_ast_node *node, Str current_namespace)
{
    switch (node->type)
    {
    case BID_AST_NODE_TYPE_INTERFACE:
        current_namespace = bid_write_interface(writer, node);
        if (node->children.length != 0)
        {
            bid_convert_message_type(writer, node, current_namespace);
            for (int i = 0; i < node->children.length; i++)
            {
                bid_write_node(writer, node->children.data[i], current_namespace);
            }
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

void bid2c(const struct bid *bid, IoWriter *writer)
{
    bid_write_header(bid, writer);
    bid_write_node(writer, bid->root_ast, str_cast(""));
}
