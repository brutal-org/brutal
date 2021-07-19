#include <bid/c_convert/bid_convert_message_type.h>
#include <bid/c_convert/bid_convert_method.h>
#include <bid/c_convert/bid_convert_type.h>

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
