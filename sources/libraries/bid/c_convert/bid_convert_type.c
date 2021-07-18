#include <bid/c_convert/bid_convert_type.h>

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
