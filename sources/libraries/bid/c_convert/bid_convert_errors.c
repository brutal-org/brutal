#include <bid/c_convert/bid_convert_type.h>

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
