
#include <bid/c_convert/bid_convert_type.h>
#include <bid/c_convert/bid_convert_typedef.h>

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

    print(writer, "; \n\n");
}
