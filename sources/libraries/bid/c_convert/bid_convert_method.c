#include <bid/c_convert/bid_convert_method.h>
#include <bid/c_convert/bid_convert_type.h>

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

// NOT IMPLEMENTED RIGHT NOW

void bid_write_method_received_structure(IoWriter *writer, struct bid_ast_node *method)
{
    print(writer, "struct {}_receive { \n", method->method.name);

    for (int i = 0; i < method->children.length; i++)
    {
        if (method->children.data[i]->type == BID_AST_NODE_TYPE_METHOD_RETURN_TYPE)
        {
            bid_write_argument(writer, method->children.data[i]);
            print(writer, "; \n");
        }
    }
    print(writer, "}; \n\n");
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
